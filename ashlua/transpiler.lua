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

local scope = {}

function Transpiler:eval_type(node, affectation)
    if affectation == nil then affectation = false end
    if node == nil then error("node parameter of eval_type is nil") end
    if node:is_type("String") then
        return 'str'
    elseif node:is_type("Boolean") then
        return 'bool'
    elseif node:is_type("Number") then
        return 'num'
    elseif node:is_type({"Integer", "Hexadecimal", "Octal", "Binary"}) then
        return 'int'
    elseif node:is_type("Identifier") then
        local id = node.left.value
        if not affectation then
            if tools.contains(scope, id) then
                return scope[id]
            else
                error("Trying to access undefined variable " .. id)
            end
        else
            return 'to_be_defined'
        end
    elseif node:is_type({"+", "-", "*", "**", "//"}) then
        local left = self:eval_type(node.left)
        local right = self:eval_type(node.right)
        if left == 'int' and right == 'int' then
            return 'int'
        elseif left == 'num' or right == 'num' then
            return 'num'
        end
    elseif node.is_type("/") then
        return 'num'
    elseif node.is_type("=") then
        local id = node.left.left.value
        local left = self:eval_type(node.left, true) -- in affectation mode, it can be undefined
        local right = self:eval_type(node.right)
        if left ~= 'to_be_defined' then
            if left ~= right then
                error("Trying to change variable " .. id .. " from " ..
                          scope[id] .. " to " .. right)
            end
            -- else : la variable est déjà enregistré et on lui affecte une valeur de son type
        else
            scope[id] = right
            -- on mémorise le type de l'identifier left
        end
    else
        error("Unknow node type : " .. node.type)
    end
end

function Transpiler:transpile(node, is_root)
    if node == nil then error("node parameter of transpile is nil") end
    if is_root == nil then is_root = false end
    local s = ""
    if is_root and not node:is_type(lang.symbols.all_affectation) then
        s = "return "
    end
    if node:is_type({"String", "Integer", "Hexadecimal", "Boolean", "Number"}) then
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
        local left_type = self:eval_type(node.left)
        local right_type = self:eval_type(node.right)
        -- int ** int => int
        if left_type == 'int' and right_type == 'int' then
            s = s .. "math.floor(" .. self:transpile(node.left) .. " ^ " ..
                    self:transpile(node.right) .. ")"
        else
            s = s .. self:transpile(node.left) .. " ^ " ..
                    self:transpile(node.right)
        end
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
