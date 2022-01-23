local yapre = yapre

local log = {}
log._print = print
log._level = 0 -- 0 info, 1 warning, 2 error

function log.enable(level)
    if level == nil then level = 0 end

    log.disable()

    if level < 1 then
        log.info = function(...) log._print("[debug info]", ...) end
    end
    if level < 2 then
        log.warning = function(...) log._print("[debug warning]", ...) end
    end

    if level < 3 then
        log.error = function(...)
            log._print("[debug error]", ...)
            yapre.dbg()
        end
    end

    print = log.info
end

function log.disable()
    log.info = function(...) end
    log.warning = function(...) end
    log.error = function(...) end
    print = log.info
end

function log.assert(condition, ...) if not condition then log.error(...) end end

log.enable(0)

return log
