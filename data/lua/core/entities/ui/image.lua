local palette_data = yapre.palette
local yecs = require("core.yecs")

-- image
local image_behavior = {}
yecs.Behavior:Register("image", image_behavior)
function image_behavior:OnInit()
    local default_image_size = {
        width = 32,
        height = 32
    }
    local default_border_size = {
        width = 36,
        height = 36
    }
    local default_image_offset = {
        x = 2,
        y = 2,
        z = 1
    }
    local border_color = palette_data.border_color
    self.sprite:AddSprite("image_border", "./image/ui/blank2.png", {
        size = default_border_size,
        color = border_color
    })

    self.sprite:AddSprite("image", "./image/ui/blank2.png", {
        size = default_image_size,
        offset = default_image_offset
    })

    return self
end

function image_behavior:SetTexture(texture)
    local image_sprite = self.sprite.sprites["image"]
    image_sprite.texture = texture
end

function image_behavior:SetTextureSize(width, height)
    local image_sprite = self.sprite.sprites["image"]
    if image_sprite then
        image_sprite.size = {
            width = width,
            height = height
        }
    end

    local border_sprite = self.sprite.sprites["image_border"]
    if border_sprite then
        border_sprite.size = {
            width = width + 4,
            height = height + 4
        }
    end
end

function image_behavior:SetBorderEnabled(enable)
    if not enable then
        self.sprite:RemoveSprite("image_border")
        return
    end

    local default_image_size = {
        width = 32,
        height = 32
    }
    local default_border_size = {
        width = 36,
        height = 36
    }
    local default_image_offset = {
        x = 2,
        y = 2,
        z = 1
    }
    local border_color = palette_data.border_color

    local image_sprite = self.sprite.sprites["image"]
    if not image_sprite then
        local size = default_border_size
    else
        local image_sprite_size = image_sprite.size
        if image_sprite_size.width == -1 or image_sprite_size.height == -1 then
            local size = self.size
        else
            local size = {
                width = image_sprite_size.width + 4,
                height = image_sprite_size.height + 4
            }
        end
    end

    self.sprite:AddSprite("image_border", "./image/ui/blank2.png", {
        size = default_border_size,
        color = border_color
    })
end

function image_behavior:AddAnimationState(key, texture_format, start_idx, end_idx, loop)
    self.animation:AddState(key, "image", texture_format, start_idx, end_idx, loop)
end
function image_behavior:RemoveAnimationState(key)
    self.animation:RemoveState(key)
end
function image_behavior:PlayAnimation(key)
    self.animation:StopAll()
    self.animation:Play(key)
end
function image_behavior:StopAnimation(key)
    self.animation:Stop(key)
end

yecs.EntityFactory:Register("image", {"position", "sprite", "size", "tick", "animation", "layout"}, {"image"})
