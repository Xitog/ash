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

-- unused. Lua returns always a float with ^
function tools.is_int(n)
    return math.floor(n) == n
end

function tools.equal(v1, v2)
    if type(v1) == "number" or type(v2) == "number" then
        return v1 == v2 and math.type(v1) == math.type(v2)
        -- ou tostring(v1) == tostring(v2) -- test 5.0 vs 5
    else
        return v1 == v2
    end
end

return tools
