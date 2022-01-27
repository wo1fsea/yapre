local yecs = require("core.yecs")

yecs.Behavior:Register("dpad_up_behavior", {
    OnClicked = function(self, x, y)
    end,
    OnInit = function(self)
        self:SetButtonResource{
            normal = {"./image/ui/dpad16/u%d.png", 1, 1},
            pressed = {"./image/ui/dpad16/u%d.png", 2, 2},
            disabled = {"./image/ui/dpad16/u%d.png", 2, 2}
        }
        self:SetState("normal")
    end
})

yecs.Behavior:Register("dpad_down_behavior", {
    OnClicked = function(self, x, y)
    end,
    OnInit = function(self)
        self:SetButtonResource{
            normal = {"./image/ui/dpad16/d%d.png", 1, 1},
            pressed = {"./image/ui/dpad16/d%d.png", 2, 2},
            disabled = {"./image/ui/dpad16/d%d.png", 2, 2}
        }
        self:SetState("normal")
    end
})

yecs.Behavior:Register("dpad_left_behavior", {
    OnClicked = function(self, x, y)
    end,
    OnInit = function(self)
        self:SetButtonResource{
            normal = {"./image/ui/dpad16/l%d.png", 1, 1},
            pressed = {"./image/ui/dpad16/l%d.png", 2, 2},
            disabled = {"./image/ui/dpad16/l%d.png", 2, 2}
        }
        self:SetState("normal")
    end
})

yecs.Behavior:Register("dpad_right_behavior", {
    OnClicked = function(self, x, y)
    end,
    OnInit = function(self)
        self:SetButtonResource{
            normal = {"./image/ui/dpad16/r%d.png", 1, 1},
            pressed = {"./image/ui/dpad16/r%d.png", 2, 2},
            disabled = {"./image/ui/dpad16/r%d.png", 2, 2}
        }
        self:SetState("normal")
    end
})

-- dpad 
local dpad_behavior = {}
yecs.Behavior:Register("dpad", dpad_behavior)
function dpad_behavior:OnInit()
    local button_up = yecs.EntityFactory:Make("button", {"dpad_up_behavior"})
    local button_down = yecs.EntityFactory:Make("button", {"dpad_down_behavior"})
    local button_left = yecs.EntityFactory:Make("button", {"dpad_left_behavior"})
    local button_right = yecs.EntityFactory:Make("button", {"dpad_right_behavior"})
    
    self.tree:AddChild(button_up)
    self.tree:AddChild(button_left)
    self.tree:AddChild(button_down)
    self.tree:AddChild(button_right)

    button_up.layout:SetCenterX(self.layout:GetCenterX(), 0)
    button_up.layout:SetBottom(self.layout:GetCenterY(), -2)

    button_down.layout:SetCenterX(self.layout:GetCenterX(), 0)
    button_down.layout:SetTop(self.layout:GetCenterY(), 2)

    button_left.layout:SetRight(self.layout:GetCenterX(), -2)
    button_left.layout:SetCenterY(self.layout:GetCenterY(), 0)

    button_right.layout:SetLeft(self.layout:GetCenterX(), 2)
    button_right.layout:SetCenterY(self.layout:GetCenterY(), 0)

    self.size = {
        width = 64 + 2,
        height = 64 + 2
    }
end

yecs.EntityFactory:Register("dpad", {"position", "sprite", "tree", "size", "input", "layout"}, {"dpad"})
