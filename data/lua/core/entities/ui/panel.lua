local palette_data = yapre.palette
local yecs = require("core.yecs")

-- panel
local panel_behavior = {}
yecs.Behavior:Register("panel", panel_behavior)
function panel_behavior:OnInit()
    local panel_size = {
        width = 64,
        height = 64
    }
    local panel_color = palette_data.colors[2]

    self.sprite:AddSprite("panel", "./image/ui/blank2.png", {
        size = panel_size,
        color = panel_color
    })
    self.size = {
        width = 64,
        height = 64
    }

    return self
end

function panel_behavior:SetSize(width, height)
    self.size.width = width
    self.size.height = height
    local sprites = self.sprite.sprites
    local sprite_size = sprites["panel"].size
    sprite_size.width = width
    sprite_size.height = height
end

yecs.EntityFactory:Register("panel", {"position", "sprite", "tree", "size", "layout"}, {"panel"})
