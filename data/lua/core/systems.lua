local systems = {}
local yecs = require("utils.yecs")

-- sprite system
local sprite_system = {}
function sprite_system:Update(delta_ms)
    local sprite_entities = self.world:GetEntities(function(entity) return entity.sprite end)
    for _, entity in pairs(sprite_entities) do
        local position = entity.position or {x=0, y=0, z=0}
        for _, sprite_data in ipairs(entity.sprite.sprites) do
            local offset = sprite_data.offset
            local size = sprite_data.size
            local color = sprite_data.color
            yapre.DrawSprite(
                sprite_data.texture, 
                position.x + offset.x, position.y + offset.y, position.z + offset.z, 
                size.width, size.heigh, 
                0., 
                color.r, color.g, color.b 
                ) 
        end
    end
end
yecs.System:Register("sprite", sprite_system)

-- input system
local input_system = {}
function input_system:Update(delta_ms)
end

function input_system:Init(delta_ms)
    function OnKey(timestamp, state, multi, keyode)
        print(string.format("%s-[OnKey] %i:%i:%i:%c", self.world, timestamp, state, multi, keyode))
        if self.OnKey then
            self:OnKey(timestamp, state, multi, keyode)
        end
    end

    function OnMouse(timestamp, state, button, x, y)
        print(string.format("%s-:[OnMouse] %i:%i:%i:(%i,%i)", self.world, timestamp, state, button, x, y))
        if self.OnMouse then
            self:OnMouse(timestamp, state, button, x, y)
        end
    end

    yapre.BindKeyboardInputCallback(string.format("%s-OnKey", self.world), OnKey)
    yapre.BindMouseInputCallback(string.format("%s-OnMouse", self.world), OnMouse)
end

function input_system:Deinit(delta_ms)
    yapre.UnbindKeyboardInputCallback(string.format("%s-OnKey", self.world))
    yapre.UnbindMouseInputCallback(string.format("%s-OnMouse", self.world))
end

yecs.System:Register("input", input_system)

return systems
