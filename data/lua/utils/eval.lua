local yapre = yapre

local cache = {}
local eval = {}

function eval.eval(v_string)
    local func = cache[v_string]
    local err = nil

    if not func then
    	func, err = load(v_string)
    end

    if func then
	cache[v_string] = func
        local result, val = pcall(func)
        if result then
            return val
        else
            yapre.log.error("eval execution error:", val)
        end
    else
        yapre.log.error("eval compilation error:", err)
    end
end

return eval
