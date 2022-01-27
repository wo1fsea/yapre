local yapre = yapre
local yecs = require("core.yecs")

-- tick system
local tick_system = {}
tick_system.update_order = 0
function tick_system:Init()
end

function tick_system:Deinit()
end

function tick_system:Update(delta_ms)
    local world = self.world
    local tick_entities = self.world:GetEntitiesWithComponent("tick")

    local g_tick_callbacks = {}
    local g_timer_callbacks = {}
    for _, entity in pairs(tick_entities) do
        local etick = entity.tick
        local timer_callbacks = etick._timer_callbacks
        local tick_callbacks = etick._tick_callbacks
        local timer_to_remove = {}

        for timer_key, callback in pairs(timer_callbacks) do
            callback.time_ms = callback.time_ms - delta_ms
            if callback.time_ms <= 0 then
                table.insert(timer_to_remove, timer_key)
                table.insert(g_timer_callbacks, callback)
            end
        end

        for _, timer_key in ipairs(timer_to_remove) do
            timer_callbacks[timer_key] = nil
        end

        for _, callback in pairs(tick_callbacks) do
            table.insert(g_tick_callbacks, callback)
        end
    end

    table.sort(g_timer_callbacks, function(t1, t2)
        if t1.time_ms < t2.time_ms then
            return true
        end
    end)

    table.sort(g_tick_callbacks, function(t1, t2)
        if t1.tick_order < t2.tick_order then
            return true
        elseif t1.tick_order > t2.tick_order then
            return false
        else
            return false
        end
    end)

    for _, callback in ipairs(g_timer_callbacks) do
        callback.callback(-callback.time_ms)
    end

    for _, callback in ipairs(g_tick_callbacks) do
        callback.callback(delta_ms)
    end

end

yecs.System:Register("tick", tick_system)
