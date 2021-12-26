local log = {}
log._print = print
log._level = 0 -- 0 info, 1 warning, 2 error

function log.enable(level)
    if level == nil then
        level = 0
    end

    log.info = function(...)
        log._print("[debug info]", ...)
    end
    log.warning = function(...)
        log._print("[debug warning]", ...)
    end
    log.error = function(...)
        log._print("[debug error]", ...)
    end
    print = log.info
end

function log.disable()
    log.info = function(...)
    end
    log.warning = function(...)
    end
    log.error = function(...)
    end
    print = log.info
end

log.enable(0)

return log
