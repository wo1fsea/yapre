local yecs = require("core.yecs")

-- joystick
local joystick_behavior = {}
yecs.Behavior:Register("joystick", joystick_behavior)
function joystick_behavior:OnInit()
    self.sprite:AddSprite("joystick_head", "./image/ui/joystick32/2.png", {
        size = {
            width = 64,
            height = 64
        },
        offset = {
            x = 0,
            y = 0,
            z = 2
        }
    })

    self.sprite:AddSprite("joystick_background", "./image/ui/joystick32/1.png", {
        size = {
            width = 64,
            height = 64
        },
        offset = {
            x = 0,
            y = 0,
            z = 1
        }
    })

    self.size.width = 64
    self.size.height = 64

    function self.input:OnTouchBegan(x, y)
        local cx = self.entity.position.x + self.entity.size.width / 2
        local cy = self.entity.position.y + self.entity.size.height / 2
        self.entity:SetJoystickHeadOffset(x - cx, y - cy)

        local on_joystick_func = self.entity.OnJoystick
        if on_joystick_func then
            local size = self.entity.size
            local w = size.width
            local h = size.height
            on_joystick_func(self.entity, 2 * (x - cx) / w, 2 * (y - cy) / h)
        end

        return true
    end
    function self.input:OnTouchMoved(x, y)
        local cx = self.entity.position.x + self.entity.size.width / 2
        local cy = self.entity.position.y + self.entity.size.height / 2
        self.entity:SetJoystickHeadOffset(x - cx, y - cy)

        local on_joystick_func = self.entity.OnJoystick
        if on_joystick_func then
            local size = self.entity.size
            local w = size.width
            local h = size.height
            on_joystick_func(self.entity, 2. * (x - cx) / w, 2. * (y - cy) / h)
        end

        return true
    end
    function self.input:OnTouchEnded(x, y)
        self.entity:SetJoystickHeadOffset(0, 0)
        local on_joystick_func = self.entity.OnJoystick
        if on_joystick_func then
            on_joystick_func(self.entity, 0, 0)
        end
    end

    return self
end

function joystick_behavior:SetJoystickHeadOffset(x, y)
    local head_sprite = self.sprite.sprites["joystick_head"]
    if head_sprite then
        head_sprite.offset = {
            x = math.max(-self.size.width / 2, math.min(self.size.width / 2, x)),
            y = math.max(-self.size.height / 2, math.min(self.size.height / 2, y)),
            z = head_sprite.offset.z
        }
    end
end

function joystick_behavior:SetJoystickSize(width, height)
    local head_sprite = self.sprite.sprites["joystick_head"]
    if head_sprite then
        head_sprite.size = {
            width = width,
            height = height
        }
    end
    local backgroud_sprite = self.sprite.sprites["joystick_background"]
    if backgroud_sprite then
        backgroud_sprite.size = {
            width = width,
            height = height
        }
    end
    self.size = {
        width = width,
        height = height
    }
end

yecs.EntityFactory:Register("joystick", {"position", "sprite", "tree", "size", "input", "layout"}, {"joystick"})
