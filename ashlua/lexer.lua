local Lexer = {}
Lexer.__index = Lexer

function Lexer.new()
    local self = setmetatable({}, Lexer)
    self.classname = 'Lexer'
    return self
end

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

local function contains(t, v)
    for _, val in ipairs(t) do if val == v then return true end end
    return false
end

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

function Lexer:lex(s)
    local i = 1
    self.raw = s
    self.tokens = {}
    -- for i = 1, string.len(s) do
    while i <= string.len(s) do
        local c = s:sub(i, i)
        if contains(digit, c) then
            i = self:read_number(i)
        elseif contains(identifier, c) then
            i = self:read_identifier(i)
        elseif contains(operators, c) then
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
    if string.len(double) == 2 and contains(operators, double) then
        table.insert(self.tokens, Token.new('Operator', double, start, 2))
        start = start + 2
    elseif contains(operators, mono) then
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
        if c == " " then break end
        length = length + 1
        i = i + 1
    end
    local value = self.raw:sub(start, start + length - 1)
    table.insert(self.tokens, Token.new('Identifier', value, start, length))
    return i
end

function Lexer:read_number(start)
    local is_float = false
    local is_hexa = false
    local length = 0
    local i = start
    if self.raw:sub(1, 1) == '0' and self.raw:sub(2, 2) == 'x' then
        is_hexa = true
        i = i + 2
        length = 2
    end
    while i <= string.len(self.raw) do
        local c = self.raw:sub(i, i)
        if contains(digit, c) then
            length = length + 1
        elseif c == '_' then
            length = length + 1
        elseif contains(hexa, c) and is_hexa then
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
    local name = (is_float and "Number") or (is_hexa and "Hexadecimal") or
                     "Integer"
    table.insert(self.tokens, Token.new(name, value, start, length))
    return i
end

return Lexer
