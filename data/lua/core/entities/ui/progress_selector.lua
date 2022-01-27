local palette_data = yapre.palette
local yecs = require("core.yecs")

-- progress_selector
local progress_selector_behavior = {}
yecs.Behavior:Register("progress_selector", progress_selector_behavior)
function progress_selector_behavior:OnInit()
    local block_width = 2
    local block_interval = 1
    local block_height = 2
    local block_height_selected = 4
    local block_num = 100
    local color = palette_data.white

    self.data.block_height = block_height
    self.data.block_height_selected = block_height_selected
    self.data.block_num = block_num

    local offset_x = block_interval
    for idx = 1, block_num, 1 do
        self.sprite:AddSprite("progress_selector_block" .. idx, "./image/ui/blank2.png", {
            size = {
                width = block_width,
                height = block_height
            },
            color = color,
            offset = {
                x = offset_x,
                y = 0,
                z = 1
            }
        })
        offset_x = offset_x + block_interval + block_width
    end

    self:SetCurrent(1)
end

function progress_selector_behavior:SetCurrent(cur_idx)
    local sprite = nil
    local block_height = self.data.block_height
    local block_height_selected = self.data.block_height_selected
    self.data.current_idx = cur_idx

    for idx = 1, self.data.block_num, 1 do
        sprite = self.sprite.sprites["progress_selector_block" .. idx]
        if sprite then
            sprite.size.height = block_height
        end
    end

    sprite = self.sprite.sprites["progress_selector_block" .. cur_idx]
    if sprite then
        sprite.size.height = block_height_selected
    end
end

function progress_selector_behavior:GetCurrent()
    return self.data.current_idx
end

function progress_selector_behavior:GetMax()
    return self.data.block_num
end

yecs.EntityFactory:Register("progress_selector", {"position", "sprite", "tree", "size", "data", "layout"}, {"progress_selector"})
