-------------------------------------------------------------------------------
-- Node class
-------------------------------------------------------------------------------

local Node = {}
Node.__index = Node

function Node.new(left, t, right)
    --print("Creating Node : " .. t)
    local self = setmetatable({}, Node)
    self.classname = 'Node'
    self.left = left
    self.type = t
    self.right = right
    return self
end

function Node:__tostring()
    local s = self.type .. "\n"
    if self.left ~= nil then
        s = s .. "\t" .. tostring(self.left) .. "\n"
    end
    if self.right ~= nil then
        s = s .. "\t" .. tostring(self.right) .. "\n"
    end
    return s
end

-------------------------------------------------------------------------------
-- Parser class
-------------------------------------------------------------------------------

local Parser = {}
Parser.__index = Parser

function Parser.new()
    local self = setmetatable({}, Parser)
    self.classname = 'Parser'
    self.root = nil
    return self
end

function Parser:info(lvl, n)
    if lvl == nil then
        lvl = 0
    end
    if n == nil then
        n = self.root
    end
    if n.classname == 'Node' then
        print(string.rep("    ", lvl) .. "Nod:" .. n.type)
        if n.left ~= nil then
            self:info(lvl + 1, n.left)
        end
        if n.right ~= nil then
            self:info(lvl + 1, n.right)
        end
    elseif n.classname == 'Token' then
        print(string.rep("    ", lvl + 1) .. "Tok:" .. tostring(n))
    end
end

function Parser:parse(tokens)
    self.tokens = tokens
    self.current = 1
    self.root = self:add()
end

function Parser:test_value(v)
    if self.current > #self.tokens then
        return false
    end
    return self.tokens[self.current].value == v
end

function Parser:test_type(t)
    if self.current > #self.tokens then
        return false
    end
    return self.tokens[self.current].type == t
end

function Parser:current()
    return self.tokens[self.current]
end

function Parser:advance()
    local t = self.tokens[self.current]
    self.current = self.current + 1
    return t
end

function Parser:add ()
    local n = self:mul()
    while self:test_value("+") do
        self:advance()
        n = Node.new(n, "+", self:mul())
    end
    return n
end

function Parser:mul()
    local n = self:litt()
    while self:test_value("*") do
        self:advance()
        n = Node.new(n, "*", self:litt())
    end
    return n
end

function Parser:litt()
    local n = nil
    if self:test_type("string") or self:test_type("Integer") then
        local t = self:advance()
        n = Node.new(t, t.type)
    end
    return n
end

return Parser
