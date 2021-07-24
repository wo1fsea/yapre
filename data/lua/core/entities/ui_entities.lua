local entities = {}

local palette_data = yapre.palette
local yecs = require("core.yecs")

-- ui
-- button
local button_behavior = {}
yecs.Behavior:Register("button", button_behavior)
function button_behavior:Init()
    self.sprite:AddSprite(
    "button", 
    "./image/ui/button16/1.png", 
    {size={width=32, height=32}})

    self.size.width = 32
    self.size.height = 32

    self.animation:AddState("normal", "button",  "./image/ui/button16/%d.png", 1, 1)
    self.animation:AddState("pressed", "button",  "./image/ui/button16/%d.png", 2, 2)
    self.animation:AddState("disabled", "button",  "./image/ui/button16/%d.png", 3, 3)
    function self.input:OnTouchBegan(x, y)
        self.entity.animation:StopAll()
        self.entity.animation:Play("pressed")
        local on_touch_began_func = self.entity.OnTouchBegan
        if on_touch_began_func then on_touch_began_func(self.entity, x, y) end
        return true
    end
    function self.input:OnTouchMoved(x, y)
        local on_touch_moved_func = self.entity.OnTouchMoved
        if on_touch_moved_func then on_touch_moved_func(self.entity, x, y) end
        return true
    end
    function self.input:OnTouchEnded(x, y)
        self.entity.animation:StopAll()
        self.entity.animation:Play("normal")
        local on_clicked_func = self.entity.OnClicked
        if on_clicked_func then on_clicked_func(self.entity, x, y) end
    end

    return self
end

function button_behavior:SetState(state)
    local animation = self.animation
    animation:Stop("normal")
    animation:Stop("pressed")
    animation:Stop("disabled")
    animation:Play(state)
end

function button_behavior:SetButtonSize(width, height)
    local image_sprite = self.sprite.sprites["button"]
    if image_sprite then
        image_sprite.size = {width=width, height=height}
    end
    self.size = {width=width, height=height}
end

yecs.EntityFactory:Register(
"button",
{"position", "sprite", "tree", "size", "input", "tick", "animation"},
{"button"}
)

-- label
local label_behavior = {}
yecs.Behavior:Register("label", label_behavior)
function label_behavior:SetText(text)
    self.text:SetText(text)
end
function label_behavior:SetMaxSize(width, height)
    self.text:SetMaxSize(width, height)
end
function label_behavior:SetFontSize(size)
    self.text:SetFontSize(size)
end
function label_behavior:GetRenderSize()
    return self.text:GetRenderSize()
end

yecs.EntityFactory:Register(
"label",
{"position", "sprite", "tree", "size", "text"},
{"label"})

-- image
local image_behavior = {}
yecs.Behavior:Register("image", image_behavior)
function image_behavior:Init()
    local default_image_size = {width=32, height=32}
    local default_border_size = {width=36, height=36}
    local default_image_offset = {x=2, y=2, z=1}
    local border_color = palette_data.border_color 
    self.sprite:AddSprite(
    "image_border", 
    "./image/ui/blank2.png", 
    {size=default_border_size, color=border_color})

    self.sprite:AddSprite(
    "image", 
    "./image/ui/blank2.png", 
    {size=default_image_size, offset=default_image_offset})

    return self
end

function image_behavior:SetTexture(texture)
    local image_sprite = self.sprite.sprites["image"]
    image_sprite.texture = texture
end

function image_behavior:SetTextureSize(width, height)
    local image_sprite = self.sprite.sprites["image"]
    if image_sprite then
        image_sprite.size = {width=width, height=height}
    end

    local border_sprite = self.sprite.sprites["image_border"]
    if border_sprite then
        border_sprite.size = {width=width+4, height=height+4}
    end
end

function image_behavior:SetBorderEnabled(enable)
    if not enable then 
        self.sprite:RemoveSprite("image_border")
        return
    end
    local image_sprite = self.sprite.sprites["image"]
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

    self.sprite:AddSprite(
    "image_border", 
    "./image/ui/blank2.png", 
    {size=size, color=border_color})
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

yecs.EntityFactory:Register(
"image",
{"position", "sprite", "size", "tick", "animation"},
{"image"})


--progress
local progress_behavior = {}
yecs.Behavior:Register("progress", progress_behavior)
function progress_behavior:Init()
    local default_size = {width=64, height=4}
    local default_border_size = {width=68, height=8}
    local border_color = palette_data.border_color 
    local background_color = palette_data.background_color
    local progress_color = palette_data.red

    self.sprite:AddSprite(
    "progress_border", 
    "./image/ui/blank2.png", 
    {size=default_border_size, color=border_color})

    self.sprite:AddSprite(
    "progress_backgroud", 
    "./image/ui/blank2.png", 
    {size=default_size, offset={x=2, y=2, z=1}, color=background_color})

    self.sprite:AddSprite(
    "progress", 
    "./image/ui/blank2.png", 
    {size=default_size, offset={x=2, y=2, z=2}, color=progress_color})

    self:SetPercent(100)
    return self
end

function progress_behavior:SetPercent(percent)
    self.data.percent = percent
    local sprites = self.sprite.sprites
    local full_width = sprites["progress_backgroud"].size.width
    if percent < 0 then percent = 0 end
    if percent > 100 then percent = 100 end
    sprites["progress"].size.width = percent * full_width // 100
end

function progress_behavior:GetPercent()
    return self.data.percent
end


yecs.EntityFactory:Register(
"progress",
{"position", "sprite", "tree", "size", "data"},
{"progress"}
)

--panel
local panel_behavior = {}
yecs.Behavior:Register("panel", panel_behavior)
function panel_behavior:Init()
    local panel_size = {width=64, height=64}
    local panel_color = palette_data.colors[2] 

    self.sprite:AddSprite(
    "panel", 
    "./image/ui/blank2.png", 
    {size=panel_size, color=panel_color})
    self.size = {width=64, height=64}

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

yecs.EntityFactory:Register(
"panel",
{"position", "sprite", "tree", "size"},
{"panel"}
)

-- palette
local palette_behavior = {}
yecs.Behavior:Register("palette", palette_behavior)
function palette_behavior:Init()
    local palette_size = 4
    local offset_x = 0
    for i, color in ipairs(palette_data.colors) do
        self.sprite:AddSprite(
        "palette"..i, 
        "./image/ui/blank2.png", 
        {size={width=palette_size, height=palette_size}, color=color, offset={x=offset_x, y=0, z=1}})
        offset_x = offset_x + palette_size
    end
end

yecs.EntityFactory:Register(
"palette",
{"position", "sprite", "tree", "size"},
{"palette"}
)

-- palette
local progress_selector_behavior = {}
yecs.Behavior:Register("progress_selector", progress_selector_behavior)
function progress_selector_behavior:Init()
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
        self.sprite:AddSprite(
        "progress_selector_block"..idx, 
        "./image/ui/blank2.png", 
        {size={width=block_width, height=block_height}, color=color, offset={x=offset_x, y=0, z=1}})
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
        sprite = self.sprite.sprites["progress_selector_block"..idx]
        if sprite then
            sprite.size.height = block_height
        end
    end

    sprite = self.sprite.sprites["progress_selector_block"..cur_idx]
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

yecs.EntityFactory:Register(
"progress_selector",
{"position", "sprite", "tree", "size", "data"},
{"progress_selector"}
)

return entities
