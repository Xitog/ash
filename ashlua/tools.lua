local tools = {}

-------------------------------------------------------------------------------
-- Require
-------------------------------------------------------------------------------

local json = require('json')

-------------------------------------------------------------------------------
-- Tools
-------------------------------------------------------------------------------

function tools.contains(t, v)
    for _, val in ipairs(t) do if val == v then return true end end
    return false
end

function tools.load_json(name)
    local f = io.open(name .. ".json", "r")
    local c = f:read("*all")
    local data = json.decode(c)
    return data
end

return tools
