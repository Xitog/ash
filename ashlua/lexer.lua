local Lexer = {}

function Lexer:new()
    print("New lexer")
    return setmetatable({}, {__index = Lexer})
end

local function contains(t, v)
    for _, val in ipairs(t) do
        if val == v then
            return true
        end
    end
    return false
end

local digit = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}
local hexa = {"A", "B", "C", "D", "E", "F", "a", "b", "c", "d", "e", "f"}
local identifier = {"_",
                    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
                    "A", "B", "C", "D", "E"}

function Lexer:lex(s)
    local i = 1
    --for i = 1, string.len(s) do
    while i <= string.len(s) do
        local c = s:sub(i, i)
        if contains(digit, c) then
            i = self:read_number(s, i)
        elseif contains(identifier, c) then
            print("id:" .. c)
            i = i + 1
        else
            print("else: " .. c)
            i = i + 1
        end
    end
end

function Lexer:read_number(s, start)
    print("Reading a number at " .. start)
    local is_float = false
    local is_hexa = false
    local length = 0
    local i = start
    if s:sub(1, 1) == '0' and s:sub(2, 2) == 'x' then
        is_hexa = true
        i = i + 2
        length = 2
    end
    print("Starting loop from " .. i .. " to " .. string.len(s))
    while i <= string.len(s) do
        local c = s:sub(i, i)
        print("Testing |" .. c .. "|")
        if contains(digit, c) then
            print("digit: " .. c)
            length = length + 1
        elseif c == '_' then
            length = length + 1
            print("digit separator: _")
        elseif contains(hexa, c) and is_hexa then
            print("hexa:" .. c)
            length = length + 1
        elseif c == '.' and not is_float then
            is_float = true
        elseif c == '.' and is_float then
            break
        else
            break
        end
        i = i + 1
    end
    print("-----------------------------------")
    print("Token number")
    print("-----------------------------------")
    print("is hexa?  | " .. tostring(is_hexa))
    print("is float? | " .. tostring(is_float))
    print("i is      | " .. i .. "/" .. string.len(s))
    print("length is | " .. length)
    print("value is  | " .. s:sub(start, start + length - 1))
    print("-----------------------------------")
    return i
end

--[[
function lexer.Token:new()
    o = {
        type = nil,
        value = nil,
        start = nil,
        line = nil
    }
    setmetatable(o, self)
    self.__index = self
    return o
end
--]]

return Lexer
