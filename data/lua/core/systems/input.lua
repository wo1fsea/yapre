local yapre = yapre
local yecs = require("core.yecs")

local emscripten_keycode_mapping = require("core.data.emscripten_keycode_mapping")

-- input system
local input_system = {
    _key_events = {},
    _mouse_events = {}
}

function input_system:Update(delta_ms)
    local input_entities = self.world:GetEntitiesWithComponent("input")
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
                    position = tree_system:GetGlobalPosition(entity)
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

    for _, key_event in ipairs(self._key_events) do
        if key_event.state == 1 then
            for _, entity in pairs(input_entities) do
                entity.input:_OnKeyPressed(key_event.keycode)
            end
        else
            for _, entity in pairs(input_entities) do
                entity.input:_OnKeyReleased(key_event.keycode)
            end
        end
    end

    self._key_events = {}
    self._mouse_events = {}
end

function input_system:Init()
    local function OnKey(timestamp, state, multi, keycode)
        if yapre.platform == "emscripten" then
            keycode = emscripten_keycode_mapping:GetKeyCode(keycode)
        end
        yapre.log.info(string.format("%s-[OnKey] %i:%i:%i:%i", self.world, timestamp, state, multi, keycode))
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
        yapre.log.info(string.format("%s-:[OnMouse] %i:%i:%i:(%i,%i)", self.world, timestamp, state, button, x, y))
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
