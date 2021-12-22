local yapre = yapre

local systems = {}
local yecs = require("core.yecs")
local debug_log = require("core.debug_log")
local emscripten_keycode_mapping = require("core.data.emscripten_keycode_mapping")

-- dummy system
local dummy_system = {}
dummy_system.update_order = 0
function dummy_system:Init()
    debug_log.log("dummy Init")
end

function dummy_system:Deinit()
    debug_log.log("dummy Deinit")
end

function dummy_system:Update(delta_ms)
    debug_log.log("dummy Update ", delta_ms)
end
yecs.System:Register("dummy", dummy_system)

-- sprite system
local sprite_system = {}
sprite_system.update_order = 2048
sprite_system.update_when_paused = true
function sprite_system:Update(delta_ms)
    local tree_system = self.world.systems["tree"]
    local sprite_entities = self.world:GetEntities(function(entity)
        return entity.sprite
    end)
    for _, entity in pairs(sprite_entities) do
        local position = entity.position
        if tree_system then
            position = tree_system:GetPosition(entity)
        end
        position = position or {
            x = 0,
            y = 0,
            z = 0
        }

        for _, sprite_data in pairs(entity.sprite.sprites) do
            local offset = sprite_data.offset
            local size = sprite_data.size
            local color = sprite_data.color
            yapre.DrawSprite(sprite_data.texture, math.floor(position.x + offset.x), math.floor(position.y + offset.y),
                math.floor(position.z + offset.z), math.floor(size.width), math.floor(size.height), 0., color.r,
                color.g, color.b)
        end
    end
end
yecs.System:Register("sprite", sprite_system)

-- input system
local input_system = {
    _key_events = {},
    _mouse_events = {}
}

function input_system:Update(delta_ms)
    local input_entities = self.world:GetEntities(function(entity)
        return entity.input
    end)
    local tree_system = self.world.systems["tree"]

    for _, mouse_event in ipairs(self._mouse_events) do
        if mouse_event.button ~= 1 then
            goto continue0
        end

        for _, entity in pairs(input_entities) do
            local einput = entity.input

            if mouse_event.state == 1 then
                local size = einput.touch_size or entity.size
                if size == nil then
                    goto continue1
                end

                local position = nil
                if tree_system then
                    position = tree_system:GetPosition(entity)
                end

                position = position or entity.position
                if position == nil then
                    goto continue1
                end
                local x = position.x
                local y = position.y
                local w = size.width
                local h = size.height

                if mouse_event.x > x and mouse_event.x < x + w and mouse_event.y > y and mouse_event.y < y + h then
                    if einput:_OnTouchBegan(mouse_event.x, mouse_event.y) then
                        einput.touched = true
                        if not einput.transparent then
                            break
                        end
                    end
                end
            elseif mouse_event.state == 2 then
                if einput.touched then
                    einput:_OnTouchEnded(mouse_event.x, mouse_event.y)
                    einput.touched = false
                end
            elseif mouse_event.state == 4 then
                if einput.touched then
                    einput:_OnTouchMoved(mouse_event.x, mouse_event.y)
                end
            end
            ::continue1::
        end
        ::continue0::
    end

    self._key_events = {}
    self._mouse_events = {}
end

function input_system:Init()
    local function OnKey(timestamp, state, multi, keycode)
        if yapre.platform == "emscripten" then
            keycode = emscripten_keycode_mapping:GetKeyCode(keycode)
        end
        debug_log.log(string.format("%s-[OnKey] %i:%i:%i:%i", self.world, timestamp, state, multi, keycode))
        table.insert(self._key_events, {
            timestamp = timestamp,
            state = state,
            multi = multi,
            keycode = keycode
        })
        if self.OnKey then
            self:OnKey(timestamp, state, multi, keycode)
        end
    end

    local function OnMouse(timestamp, state, button, x, y)
        debug_log.log(string.format("%s-:[OnMouse] %i:%i:%i:(%i,%i)", self.world, timestamp, state, button, x, y))
        table.insert(self._mouse_events, {
            timestamp = timestamp,
            state = state,
            button = button,
            x = x,
            y = y
        })
        if self.OnMouse then
            self:OnMouse(timestamp, state, button, x, y)
        end
    end

    yapre.BindKeyboardInputCallback(string.format("%s-OnKey", self.world), OnKey)
    yapre.BindMouseInputCallback(string.format("%s-OnMouse", self.world), OnMouse)
end

function input_system:Deinit()
    yapre.UnbindKeyboardInputCallback(string.format("%s-OnKey", self.world))
    yapre.UnbindMouseInputCallback(string.format("%s-OnMouse", self.world))
end

yecs.System:Register("input", input_system)

-- tree_system
local tree_system = {}
tree_system.update_order = sprite_system.update_order - 1
tree_system.global_position = {}

function tree_system:_UpdateTreeNodePos(node, parent_pos)
    local pos = node.position
    local node_pos = {
        x = pos.x + parent_pos.x,
        y = pos.y + parent_pos.y,
        z = pos.z + parent_pos.z
    }
    self.global_position[node.key] = node_pos

    for _, c in pairs(node.tree.children) do
        self:_UpdateTreeNodePos(c, node_pos)
    end
end

function tree_system:Update(delta_ms)
    self.global_position = {}
    local world = self.world
    local tree_entities = self.world:GetEntities(function(entity)
        return entity.tree
    end)
    for _, entity in pairs(tree_entities) do
        local parent = entity.tree.parent
        if parent == nil then
            self:_UpdateTreeNodePos(entity, {
                x = 0,
                y = 0,
                z = 0
            })
        else
            if world.entities[parent.key] == nil then
                world:RemoveEntity(entity)
            end
        end
    end
end

function tree_system:Init()
end

function tree_system:Deinit()
end

function tree_system:GetPosition(entity)
    local position = self.global_position[entity.key]
    if position then
        return position
    else
        return entity.position
    end
end

yecs.System:Register("tree", tree_system)

-- tick system
local tick_system = {}
tick_system.update_order = 0
function tick_system:Init()
end

function tick_system:Deinit()
end

function tick_system:Update(delta_ms)
    local world = self.world
    local tick_entities = self.world:GetEntities(function(entity)
        return entity.tick
    end)
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

return systems
