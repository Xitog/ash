-------------------------------------------------------------------------------
-- Transpiler class
-------------------------------------------------------------------------------
local Transpiler = {}
Transpiler.__index = Transpiler

function Transpiler.new()
    local self = setmetatable({}, Transpiler)
    self.classname = 'Transpiler'
    self.root = nil
    return self
end

function Transpiler:transpile(node)
    if node == nil then error("node parameter of transpile is nil") end
    local s = nil
    if node.type == "Integer" then
        s = tostring(node.left.value)
    elseif node.type == "+" then
        s = self:transpile(node.left) .. " + " .. self:transpile(node.right)
    elseif node.type == "-" then
        s = self:transpile(node.left) .. " - " .. self:transpile(node.right)
    elseif node.type == "*" then
        s = self:transpile(node.left) .. " * " .. self:transpile(node.right)
    elseif node.type == "/" then
        s = self:transpile(node.left) .. " / " .. self:transpile(node.right)
    else
        error("Unknown type " .. node.type)
    end
    return s
end

return Transpiler
