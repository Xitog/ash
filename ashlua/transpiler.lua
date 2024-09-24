-------------------------------------------------------------------------------
-- Require
-------------------------------------------------------------------------------
local tools = require('tools')
local lang = tools.load_json('ash')

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

function Transpiler:transpile(node, is_root)
    if node == nil then error("node parameter of transpile is nil") end
    if is_root == nil then is_root = false end
    local s = ""
    if is_root and not node:is_type(lang.symbols.all_affectation) then
        s = "return "
    end
    if node:is_type({"String", "Integer", "Hexadecimal", "Boolean"}) then
        s = s .. tostring(node.left.value)
    elseif node:is_type("Octal") then
        s = s .. tostring(tonumber(string.sub(node.left.value, 3), 8))
    elseif node:is_type("Binary") then
        s = s .. tostring(tonumber(string.sub(node.left.value, 3), 2))
    elseif node.type == "Identifier" then
        s = s .. node.left.value
    elseif node.type == "+" then
        s = s .. self:transpile(node.left) .. " + " ..
                self:transpile(node.right)
    elseif node.type == "-" then
        s = s .. self:transpile(node.left) .. " - " ..
                self:transpile(node.right)
    elseif node.type == "*" then
        s = s .. self:transpile(node.left) .. " * " ..
                self:transpile(node.right)
    elseif node.type == "/" then
        s = s .. self:transpile(node.left) .. " / " ..
                self:transpile(node.right)
    elseif node.type == "**" then
        s = s .. self:transpile(node.left) .. " ^ " ..
                self:transpile(node.right)
            elseif node.type == "//" then
                s = s .. self:transpile(node.left) .. " // " ..
                        self:transpile(node.right)
    elseif node.type == "=" then
        s = s .. self:transpile(node.left) .. " = " ..
                self:transpile(node.right)
    elseif tools.contains(lang.symbols.combined_affectation, node.type) then
        local operator = string.sub(node.type, 1, 1)
        s =
            s .. self:transpile(node.left) .. " = " .. self:transpile(node.left) ..
                " " .. operator .. " " .. self:transpile(node.right)
    else
        error("Unknown type " .. node.type)
    end
    if is_root and node:is_type(lang.symbols.all_affectation) then
        s = s .. "\nreturn " .. self:transpile(node.left)
    end
    return s
end

return Transpiler
