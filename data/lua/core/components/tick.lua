local yapre = yapre

local yecs = require("core.yecs")

local default_tick_order = 1
yecs.Component:Register("tick", {
    _tick_callbacks = {}, -- callback(delta_ms)
    _timer_callbacks = {}, -- callback(timeover_ms)
    _operations = {
        AddTick = function(self, key, callback, tick_order)
            self._tick_callbacks[key] = {
                tick_order = tick_order or default_tick_order,
                callback = callback,
                entity = self.entity
            }
        end,
        RemoveTick = function(self, key)
            self._tick_callbacks[key] = nil
        end,
        AddTimer = function(self, key, time_ms, callback)
            self._timer_callbacks[key] = {
                time_ms = time_ms,
                callback = callback,
                entity = self.entity
            }
        end,
        RemoveTimer = function(self, key)
            self._timer_callbacks[key] = nil
        end

    }
})
