-------------------------------------------------------------------------------
-- Require
-------------------------------------------------------------------------------
local tools = require('tools')
local lang = tools.load_json('ash')

-------------------------------------------------------------------------------
-- Constants
-------------------------------------------------------------------------------

local digit = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}
local hexa = {"A", "B", "C", "D", "E", "F", "a", "b", "c", "d", "e", "f"}
local identifier = {
    "_", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n",
    "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "A", "B", "C",
    "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R",
    "S", "T", "U", "V", "W", "X", "Y", "Z"
}
local operators = {
    "+", "-", "*", "/", "%", "**", "//", "=", "+=", "-=", "*=", "/=", "%=",
    "**=", "//=", "==", "!=", ">", ">=", "=<", "<", "."
}

-------------------------------------------------------------------------------
-- Token class
-------------------------------------------------------------------------------

local Token = {}
Token.__index = Token

function Token.new(t, v, s, l)
    local self = setmetatable({}, Token)
    self.classname = 'Token'
    self.type = t
    self.value = v
    self.start = s
    self.length = l
    return self
end

function Token:__tostring()
    return self.type .. "('" .. self.value .. "', " .. self.start .. " to " ..
               (self.start + self.length - 1) .. ", #" .. self.length .. ")"
end

-------------------------------------------------------------------------------
-- Lexer class
-------------------------------------------------------------------------------

local Lexer = {}
Lexer.__index = Lexer

function Lexer.new()
    local self = setmetatable({}, Lexer)
    self.classname = 'Lexer'
    self.tokens = {}
    return self
end

function Lexer:tokens() return self.tokens end

function Lexer:lex(s)
    local i = 1
    self.raw = s
    self.tokens = {}
    -- for i = 1, string.len(s) do
    while i <= string.len(s) do
        local c = s:sub(i, i)
        if tools.contains(digit, c) then
            i = self:read_number(i)
        elseif tools.contains(identifier, c) then
            i = self:read_identifier(i)
        elseif tools.contains(operators, c) then
            i = self:read_operator(i)
        elseif c == '"' then
            i = self:read_string(i)
        elseif c == " " then
            i = i + 1
        else
            print("else: |" .. c .. "|")
            i = i + 1
        end
    end
    return self.tokens
end

function Lexer:info()
    for i, t in ipairs(self.tokens) do print(i .. ". " .. tostring(t)) end
end

function Lexer:read_string(start)
    local nb = 0
    local length = 0
    local i = start
    local escape = false
    while i <= string.len(self.raw) and nb < 2 do
        local c = self.raw:sub(i, i)
        if c == '\\' then
            escape = true
        elseif c == '"' and not escape then
            nb = nb + 1
        elseif escape then
            escape = false
        end
        length = length + 1
        i = i + 1
    end
    if nb ~= 2 then error("Unfinished string") end
    local value = self.raw:sub(start, start + length - 1)
    table.insert(self.tokens, Token.new('String', value, start, length))
    return i
end

function Lexer:read_operator(start)
    local mono = self.raw:sub(start, start)
    local double = self.raw:sub(start, start + 1)
    -- double == mono if self.raw not long enough
    if string.len(double) == 2 and tools.contains(operators, double) then
        table.insert(self.tokens, Token.new('Operator', double, start, 2))
        start = start + 2
    elseif tools.contains(operators, mono) then
        table.insert(self.tokens, Token.new('Operator', mono, start, 1))
        start = start + 1
    end
    return start
end

function Lexer:read_identifier(start)
    local length = 0
    local i = start
    while i <= string.len(self.raw) do
        local c = self.raw:sub(i, i)
        if tools.contains(identifier, c) or tools.contains(digit, c) then
            length = length + 1
            i = i + 1
        else
            break
        end
    end
    local value = self.raw:sub(start, start + length - 1)
    local ttype = 'Identifier'
    if tools.contains(lang.symbols.booleans, value) then
        ttype = 'Boolean'
    end
    table.insert(self.tokens, Token.new(ttype, value, start, length))
    return i
end

function Lexer:read_number(start)
    local is_float = false
    local base = 10
    local length = 0
    local i = start
    if self.raw:sub(1, 1) == '0' then
        if self.raw:sub(2, 2) == 'x' then
            base = 16
        elseif self.raw:sub(2,2) == 'c' then
            base = 8
        elseif self.raw:sub(2,2) == 'b' then
            base = 2
        end
        if base ~= 10 then
            i = i + 2
            length = 2
        end
    end
    while i <= string.len(self.raw) do
        local c = self.raw:sub(i, i)
        if tools.contains(digit, c) then
            if tonumber(c) >= base then
                error("Wrong digit for base " .. base .. " in " .. self.raw:sub(start, i))
            end
            length = length + 1
        elseif c == '_' then
            length = length + 1
        elseif tools.contains(hexa, c) and base == 16 then
            length = length + 1
        elseif c == '.' and not is_float then
            is_float = true
            length = length + 1
        elseif c == '.' and is_float then
            break
        else
            break
        end
        i = i + 1
    end
    local value = self.raw:sub(start, start + length - 1)
    local name = (is_float and "Number") or ((base == 16) and "Hexadecimal") or
                    ((base == 8) and "Octal") or ((base == 2) and "Binary") or
                     "Integer"
    table.insert(self.tokens, Token.new(name, value, start, length))
    return i
end

return Lexer
