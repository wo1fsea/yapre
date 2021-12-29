local yapre = yapre

local cache = {}
local eval = {}

function eval.eval(v_string)
    local func = cache[v_string]
    local err = nil

    if not func then
        func, err = load(v_string)
    end

    yapre.log.assert(func, "eval compilation error: ", err)
    cache[v_string] = func
    local result, val = pcall(func)
    yapre.log.assert(result, "eval execution error: ", val)
    return val
end

return eval
