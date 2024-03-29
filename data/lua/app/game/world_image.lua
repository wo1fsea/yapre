local world_image = {}

local core = require("core")
local yecs = core.yecs

local panel_width = 320
local panel_height = 240

yecs.Behavior:Register("world_image_button1_behavior", {
    OnClicked = function(self, x, y)
        self.world:GetEntityByTags({"image2"}):PlayAnimation("die")
        self.world:GetEntityByTags({"button2"}):SetState("normal")
        self:SetState("disabled")
    end,
    OnInit = function(self)
        self:AddComponent("tags")
        self.tags["button1"] = true
    end
})

yecs.Behavior:Register("world_image_button2_behavior", {
    OnClicked = function(self, x, y)
        self.world:GetEntityByTags({"image2"}):PlayAnimation("live")
        self.world:GetEntityByTags({"button1"}):SetState("normal")
        self:SetState("disabled")
    end,
    OnInit = function(self)
        self:AddComponent("tags")
        self.tags["button2"] = true
    end
})

yecs.Behavior:Register("world_image_image2_behavior", {
    OnInit = function(self)
        self:AddComponent("tags")
        self.tags["image2"] = true
    end
})

function world_image:Make()
    local world = yecs.World:New("world_image")
    world:AddSystemsByKeys({"sprite", "input", "tree", "tick", "layout"})
    
    local canvas = yecs.EntityFactory:Make("panel")
    world:AddEntity(canvas)
    canvas:SetSize(panel_width, panel_height)
    canvas.layout:SetCenterX(world:GetRoot().layout:GetCenterX(), 0)
    canvas.layout:SetCenterY(world:GetRoot().layout:GetCenterY(), 0)

    local image1 = yecs.EntityFactory:Make("image")
    image1.position = {
        x = 8,
        y = 8,
        z = 1
    }
    image1:SetTextureSize(128, 128)
    image1:AddAnimationState("loop", "./image/animation/wizard/%d.png", 1, 16, true)
    image1:PlayAnimation("loop")

    local image2 = yecs.EntityFactory:Make("image", {"world_image_image2_behavior"})
    image2.position = {
        x = 8 + 128 + 8,
        y = 8 + 32 + 8,
        z = 1
    }
    image2:SetTextureSize(128, 128)
    image2:SetTexture("./image/animation/blood/1.png")

    image2:AddAnimationState("die", "./image/animation/blood/%d.png", 1, 20)
    image2:AddAnimationState("live", "./image/animation/blood/%d.png", 1, 1)

    local button1 = yecs.EntityFactory:Make("button", {"world_image_button1_behavior"})
    local button2 = yecs.EntityFactory:Make("button", {"world_image_button2_behavior"})

    button1.position = {
        x = 8 + 8 + 128 + 32,
        y = 8 + 32 + 8 + 128 + 4,
        z = 1
    }
    button2.position = {
        x = 8 + 8 + 128,
        y = 8 + 32 + 8 + 128 + 4,
        z = 1
    }

    button1:SetState("normal")
    button2:SetState("disabled")

    canvas.tree:AddChild(button1)
    canvas.tree:AddChild(button2)
    canvas.tree:AddChild(image1)
    canvas.tree:AddChild(image2)

    return world
end

return world_image
