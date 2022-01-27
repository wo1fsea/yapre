local yecs = require("core.yecs")

-- ui
-- button
local button_behavior = {}
yecs.Behavior:Register("button", button_behavior)
function button_behavior:OnInit()
    self.sprite:AddSprite("button", "./image/ui/button16/1.png", {
        size = {
            width = 32,
            height = 32
        }
    })

    self.size.width = 32
    self.size.height = 32

    self.animation:AddState("normal", "button", "./image/ui/button16/%d.png", 1, 1)
    self.animation:AddState("pressed", "button", "./image/ui/button16/%d.png", 2, 2)
    self.animation:AddState("disabled", "button", "./image/ui/button16/%d.png", 3, 3)
    function self.input:OnTouchBegan(x, y)
        self.entity.animation:StopAll()
        self.entity.animation:Play("pressed")
        local on_touch_began_func = self.entity.OnTouchBegan
        if on_touch_began_func then
            on_touch_began_func(self.entity, x, y)
        end
        return true
    end
    function self.input:OnTouchMoved(x, y)
        local on_touch_moved_func = self.entity.OnTouchMoved
        if on_touch_moved_func then
            on_touch_moved_func(self.entity, x, y)
        end
        return true
    end
    function self.input:OnTouchEnded(x, y)
        self.entity.animation:StopAll()
        self.entity.animation:Play("normal")
        local on_clicked_func = self.entity.OnClicked
        if on_clicked_func then
            on_clicked_func(self.entity, x, y)
        end
    end
    function self.input.OnKeyPressed(self, keycode)
        local bind_key = self.entity.data.button_bind_key
        if bind_key and keycode == string.byte(bind_key) then
            self:OnTouchBegan(0, 0)
        end
    end
    function self.input.OnKeyReleased(self, keycode)
        local bind_key = self.entity.data.button_bind_key
        if bind_key and keycode == string.byte(bind_key) then
            self:OnTouchEnded(0, 0)
        end
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
        image_sprite.size = {
            width = width,
            height = height
        }
    end
    self.size = {
        width = width,
        height = height
    }
end

function button_behavior:BindKey(key)
    self.data.button_bind_key = key
end

yecs.EntityFactory:Register("button", {"position", "sprite", "tree", "size", "input", "tick", "animation", "data", "layout"},
    {"button"})
