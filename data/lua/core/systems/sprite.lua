local yapre = yapre
local yecs = require("core.yecs")

-- sprite system
local sprite_system = {}
sprite_system.update_order = 2048
sprite_system.update_when_paused = true
function sprite_system:Update(delta_ms)
    local tree_system = self.world.systems["tree"]
    local sprite_entities = self.world:GetEntitiesWithComponent("sprite")

    for _, entity in pairs(sprite_entities) do
        local position = entity.position
        if tree_system then
            position = tree_system:GetGlobalPosition(entity)
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
            yapre.DrawSprite(sprite_data.texture, {position.x + offset.x, position.y + offset.y, position.z + offset.z},
                {size.width, size.height}, 0., {color.r, color.g, color.b})
        end
    end
end
yecs.System:Register("sprite", sprite_system)
