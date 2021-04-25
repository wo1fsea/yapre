local entities = {}
local yecs = require("core.yecs")

-- ui
-- button
yecs.EntityFactory:Register(
"button",
{"position", "sprite", "tree", "size", "input", "animation"},
function(button)
    button.sprite:AddSprite(
    "button", 
    "data/image/ui/button16/1.png", 
    {size={width=32, height=32}})
    button.size.width = 32
    button.size.height = 32

    button.animation:AddState("normal", "button",  "data/image/ui/button16/%d.png", 1, 1)
    button.animation:AddState("pressed", "button",  "data/image/ui/button16/%d.png", 2, 2)
    button.animation:AddState("disabled", "button",  "data/image/ui/button16/%d.png", 3, 3)
    function button.input:OnTouchBegan(x, y)
        self.entity.animation:Play("pressed")
        local on_touch_began_func = self.entity.OnTouchBegan
        if on_touch_began_func then on_touch_began_func(self.entity, x, y) end
        return true
    end
        function button.input:OnTouchMoved(x, y)
        local on_touch_moved_func = self.entity.OnTouchMoved
        if on_touch_moved_func then on_touch_moved_func(self.entity, x, y) end
        return true
    end
    function button.input:OnTouchEnded(x, y)
        self.entity.animation:Play("normal")
        local on_clicked_func = self.entity.OnClicked
        if on_clicked_func then on_clicked_func(self.entity, x, y) end
    end

    function button:SetState(state)
        local animation = self.animation
        animation:Stop("normal")
        animation:Stop("pressed")
        animation:Stop("disabled")
        animation:Play(state)
    end

    return button
end
)

-- label
yecs.EntityFactory:Register(
"label",
{"position", "sprite", "tree", "size", "text"},
function(label)
    function label:SetText(text)
        self.text:SetText(text)
    end
    function label:SetMaxSize(width, height)
        self.text:SetMaxSize(width, height)
    end
end)

-- image
yecs.EntityFactory:Register(
"image",
{"position", "sprite", "size", "animation"},
function(image)
    local default_image_size = {width=32, height=32}
    local default_border_size = {width=36, height=36}
    local default_image_offset = {x=2, y=2, z=1}
    local border_color = {r=1, g=1, b=1}
    image.sprite:AddSprite(
    "image_border", 
    "data/image/ui/blank2.png", 
    {size=default_border_size, color=border_color})
    
    image.sprite:AddSprite(
    "image", 
    "data/image/ui/blank2.png", 
    {size=default_image_size, offset=default_image_offset})

    function image:SetTexture(texture)
        local image_sprite = self.sprite.sprites["image"]
        image_sprite.texture = texture
    end

    function image:SetTextureSize(width, height)
        local image_sprite = self.sprite.sprites["image"]
        if image_sprite then
            image_sprite.size = {width=width, height=height}
        end

        local border_sprite = self.sprite.sprites["image_border"]
        if border_sprite then
            border_sprite.size = {width=width+4, height=height+4}
        end
    end

    function image:SetBorderEnabled(enable)
        if not enable then 
            self.sprite:RemoveSprite("image_border")
            return
        end
        local image_sprite = button.sprite.sprites["image"]
        if not image_sprite then
            local size = default_border_size
        else
            local image_sprite_size = image_sprite.size
            if image_sprite_size.width == -1 or image_sprite_size.height == -1 then
                local size = self.size
            else
                local size = {width=image_sprite_size.width+4, height=image_sprite_size.height+4}
            end
        end

        button.sprite:AddSprite(
        "image_border", 
        "data/image/ui/blank2.png", 
        {size=size, color=border_color})
    end

    function image:AddAnimationState(key, texture_format, start_idx, end_idx)
        self.animation:AddState(key, "image", texture_format, start_idx, end_idx)
    end
    function image:RemoveAnimationState(key)
        self.animation:RemoveState(key)
    end
    function image:PlayAnimation(key)
        self.animation:StopAll() 
        self.animation:Play(key)
    end
    function image:StopAnimation(key)
        self.animation:Stop(key) 
    end
    
    return image
end
)

--progress
yecs.EntityFactory:Register(
"progress",
{"position", "sprite", "size"},
function(progress)
    local default_size = {width=64, height=4}
    local default_border_size = {width=68, height=8}
    local border_color = {r=1, g=1, b=1}
    local background_color = {r=0, g=0, b=0}
    local progress_color = {r=0.25, g=0.15, b=1}
    progress.sprite:AddSprite(
    "progress_border", 
    "data/image/ui/blank2.png", 
    {size=default_border_size, color=border_color})

    progress.sprite:AddSprite(
    "progress_backgroud", 
    "data/image/ui/blank2.png", 
    {size=default_size, offset={x=2, y=2, z=1}, color=background_color})
    
    progress.sprite:AddSprite(
    "progress", 
    "data/image/ui/blank2.png", 
    {size=default_size, offset={x=2, y=2, z=2}, color=progress_color})
    
    function progress:SetPercent(percent)
        local sprites = self.sprite.sprites
        local full_width = sprites["progress_backgroud"].size.width
        if percent < 0 then percent = 0 end
        if percent > 100 then percent = 100 end
        sprites["progress"].size.width = percent * full_width // 100
    end

    return progress
end
)

--panel

return entities
