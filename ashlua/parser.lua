-------------------------------------------------------------------------------
-- Node class
-------------------------------------------------------------------------------
local Node = {}
Node.__index = Node

function Node.new(left, t, right)
    -- print("Creating Node : " .. t)
    local self = setmetatable({}, Node)
    self.classname = 'Node'
    self.left = left
    self.type = t
    self.right = right
    return self
end

function Node:is_type(ts)
    if type(ts) == "table" then
        for _, t in ipairs(ts) do if t == self.type then return true end end
    else
        return self.type == ts
    end
    return false
end

function Node:__tostring()
    local s = self.type .. "\n"
    if self.left ~= nil then s = s .. "\t" .. tostring(self.left) .. "\n" end
    if self.right ~= nil then s = s .. "\t" .. tostring(self.right) .. "\n" end
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

function Parser:info(lvl, n, side)
    if lvl == nil then lvl = 0 end
    if n == nil then n = self.root end
    if side == nil then side = 'root' end
    if n.classname == 'Node' then
        print(string.rep("    ", lvl) .. "Nod:" .. n.type .. " (" .. side .. ")")
        if n.left ~= nil then self:info(lvl + 1, n.left, "left") end
        if n.right ~= nil then self:info(lvl + 1, n.right, "right") end
    elseif n.classname == 'Token' then
        print(string.rep("    ", lvl + 1) .. "Tok:" .. tostring(n) .. " (" ..
                  side .. ")")
    end
end

function Parser:parse(tokens)
    self.tokens = tokens
    self.current = 1
    self.root = self:affectation()
    self.scope = {}
end

function Parser:test_value(v, i)
    if i == nil then i = self.current end
    if i > #self.tokens then return false end
    return self.tokens[i].value == v
end

function Parser:test_values(vs, i)
    if i == nil then i = self.current end
    for _, v in ipairs(vs) do if self:test_value(v, i) then return true end end
    return false
end

function Parser:test_type(t)
    if self.current > #self.tokens then return false end
    return self.tokens[self.current].type == t
end

function Parser:test_types(ts, i)
    if i == nil then i = self.current end
    for _, v in ipairs(ts) do if self:test_type(v, i) then return true end end
    return false
end

function Parser:current() return self.tokens[self.current] end

function Parser:advance()
    local t = self.tokens[self.current]
    self.current = self.current + 1
    return t
end

function Parser:affectation()
    --print('affectation test')
    local n = nil
    while self:test_values({"=", "+=", "-=", "*=", "%=", "/=", "**=", "//="}, self.current + 1) do
        print('affectation ok')
        if not self:test_type("Identifier") then
            error("Only identifier on left side of affectation")
        end
        n = Node.new(self:advance(), 'Identifier')
        local op = self:advance()
        n = Node.new(n, op.value, self:affectation())
    end
    if n == nil then
        --print('making node')
        n = self:addsub()
    end
    return n
end

function Parser:addsub()
    local n = self:muldiv()
    while self:test_value("+") or self:test_value("-") do
        local t = self:advance()
        n = Node.new(n, t.value, self:muldiv())
    end
    return n
end

function Parser:muldiv()
    local n = self:powintdiv()
    while self:test_value("*") or self:test_value("/") do
        local t = self:advance()
        n = Node.new(n, t.value, self:powintdiv())
    end
    return n
end

function Parser:powintdiv()
    local n = self:litt()
    local t = nil
    local next = nil
    while self:test_value("**") or self:test_value("//") do
        t = self:advance()
        next = self:powintdiv()
    end
    if next == nil then
        return n
    else
        return Node.new(n, t.value, next)
    end
end

function Parser:litt()
    local n = nil
    if self:test_types({"Hexadecimal", "Octal", "Binary", "Integer", "Boolean", "String", "Identifier"}) then
        local t = self:advance()
        n = Node.new(t, t.type)
    else
        error("Unknow token type : " .. tostring(self.tokens[self.current]) .. " at " .. self.current)
    end
    return n
end

return Parser
