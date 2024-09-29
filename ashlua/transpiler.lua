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

local eval_table = {
    ['int'] = {
        {
            ['if_operator'] = {'+', '-', '*', '//', '**'},
            ['if_type'] = {
                ['num'] = 'num',
                ['int'] = 'int'
            }
        },
        {
            ['if_operator'] = {'/'},
            ['if_type'] = {
                ['num'] = 'num',
                ['int'] = 'num'
            }
        }
    },
    ['num'] = {
        {
            ['if_operator'] = {'+', '-', '*', '//', '**', '/'},
            ['if_type'] = {
                ['num'] = 'num',
                ['int'] = 'num'
            }
        }
    },
    ['str'] = {
        {
            ['if_operator'] = {'+'},
            ['if_type'] = {
                ['str'] = 'str'
            }
        },
        {
            ['if_operator'] = {'*'},
            ['if_type'] = {
                ['int'] = 'str'
            }
        }
    }
}

function Transpiler.get_type(t1, op, t2)
    local type1 = eval_table[t1]
    if type1 == nil then
        error("Type " .. t1 .. " unknown.")
    end
    local res = nil
    for _, rule in ipairs(type1) do
        if tools.contains(rule['if_operator'], op) then
            for type2, r in pairs(rule['if_type']) do
                if type2 == t2 then
                    res = r
                    break
                end
            end
        end
    end
    if res == nil then
        error("No rule found for type1 = " .. t1 .. " and op = " .. op .. " and type2 = " .. t2)
    end
    return res
end

function Transpiler:eval_type(node, affectation)
    --print("eval_type: " .. (tostring(node) or 'nil'))
    --print("eval_type: " .. node.type)
    if node == nil then
        error("Trying to evaluate type of nil node")
    end
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
            if tools.has_key(scope, id) then
                return scope[id]
            else
                error("Trying to access undefined variable " .. id)
            end
        else
            return 'to_be_defined'
        end
    elseif node:is_type({"+", "-", "*", "/", "**", "//"}) then
        local left = self:eval_type(node.left)
        local right = self:eval_type(node.right)
        return Transpiler.get_type(left, node.type, right)
    elseif node:is_type("=") then
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
        return scope[id]
    elseif node:is_type({"+=", "-=", "*=", "/=", "%=", "**=", "//="}) then
        local left = self:eval_type(node.left)
        local right = self:eval_type(node.right)
        local op = string.sub(node.type, 1, 1)
        return Transpiler.get_type(left, op, right)
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
    elseif node:is_type({"+", "-", "*", "/"}) then
        local left_t = self:eval_type(node.left)
        local right_t = self:eval_type(node.right)
        local left = self:transpile(node.left)
        local right = self:transpile(node.right)
        if left_t == 'int' or left_t == 'num' then
            s = s .. left .. " " .. node.type .. " " .. right
        elseif left_t == 'str' and right_t == 'str' and node.type == '+' then
            s = s .. left .. " .. " .. right
        elseif left_t == 'str' and right_t == 'int' and node.type == '*' then
            s = s .. "string.rep(" .. left .. ", " .. right .. ")"
        else
            error("Unknown method for type " .. left_t .. " and operator + with operand type " .. right_t)
        end
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
