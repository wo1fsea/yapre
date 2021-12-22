local debug_log = {}
debug_log._print = print

function debug_log.enable()
    debug_log.log = function(...)
        debug_log._print("[debug log]", ...)
    end
    print = debug_log.log
end

function debug_log.disable()
    debug_log.log = function()
    end
    print = debug_log._print
end

debug_log.enable()

return debug_log
