local palette_data = yapre.palette
local yecs = require("core.yecs")

-- palette
local palette_behavior = {}
yecs.Behavior:Register("palette", palette_behavior)
function palette_behavior:OnInit()
    local palette_size = 8
    local offset_x = 0
    for i, color in ipairs(palette_data.colors) do
        self.sprite:AddSprite("palette" .. i, "./image/ui/blank2.png", {
            size = {
                width = palette_size,
                height = palette_size
            },
            color = color,
            offset = {
                x = offset_x,
                y = 0,
                z = 1
            }
        })
        offset_x = offset_x + palette_size
    end
    self.size = {
        width = offset_x + palette_size,
        height = palette_size
    }
end

yecs.EntityFactory:Register("palette", {"position", "sprite", "tree", "size", "layout"}, {"palette"})
