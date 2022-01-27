local palette_data = yapre.palette
local yecs = require("core.yecs")

-- progress
local progress_behavior = {}
yecs.Behavior:Register("progress", progress_behavior)
function progress_behavior:OnInit()
    local default_size = {
        width = 64,
        height = 4
    }
    local default_border_size = {
        width = 68,
        height = 8
    }
    local border_color = palette_data.border_color
    local background_color = palette_data.background_color
    local progress_color = palette_data.red

    self.sprite:AddSprite("progress_border", "./image/ui/blank2.png", {
        size = default_border_size,
        color = border_color
    })

    self.sprite:AddSprite("progress_backgroud", "./image/ui/blank2.png", {
        size = default_size,
        offset = {
            x = 2,
            y = 2,
            z = 1
        },
        color = background_color
    })

    self.sprite:AddSprite("progress", "./image/ui/blank2.png", {
        size = default_size,
        offset = {
            x = 2,
            y = 2,
            z = 2
        },
        color = progress_color
    })

    self:SetPercent(100)
    return self
end

function progress_behavior:SetPercent(percent)
    self.data.percent = percent
    local sprites = self.sprite.sprites
    local full_width = sprites["progress_backgroud"].size.width
    if percent < 0 then
        percent = 0
    end
    if percent > 100 then
        percent = 100
    end
    sprites["progress"].size.width = percent * full_width // 100
end

function progress_behavior:GetPercent()
    return self.data.percent
end

yecs.EntityFactory:Register("progress", {"position", "sprite", "tree", "size", "data", "layout"}, {"progress"})
