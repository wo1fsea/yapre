local systems = {}
local yecs = require("utils.yecs")

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

return systems
