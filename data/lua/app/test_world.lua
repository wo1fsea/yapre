local test_world = {}

local core = require("core")
local yecs = core.yecs

yecs.Behavior:Register("increase_progress_behavior", {
    OnInit=function(self)
        self:AddComponent("tick")
        local progress_delta = 1
        self.tick:AddTick("increase_progress", 
        function() 
            local percent = self:GetPercent()
            if percent == 100 then 
                progress_delta = -1
            elseif percent == 0 then
                progress_delta = 1
            end
            self:SetPercent(percent+progress_delta)
        end)
    end,
})

yecs.Behavior:Register("button1_behavior", {
    OnClicked=function(self, x, y)
        self.world:GetEntityByTags({"image"}):PlayAnimation("die")
        self.world:GetEntityByTags({"button2"}):SetState("normal")
        self:SetState("disabled")
    end,
    OnInit=function(self)
        self:AddComponent("tags")
        self.tags["button1"] = true
    end
})

yecs.Behavior:Register("button2_behavior", {
    OnClicked=function(self, x, y)
        self.world:GetEntityByTags({"image"}):PlayAnimation("live")
        self.world:GetEntityByTags({"button1"}):SetState("normal")
        self:SetState("disabled")
    end,
    OnInit=function(self)
        self:AddComponent("tags")
        self.tags["button2"] = true
    end
})

yecs.Behavior:Register("image_behavior", {
    OnInit=function(self)
        self:AddComponent("tags")
        self.tags["image"] = true
    end
})

function test_world:New()
    local world = yecs.World:New("test_world")
    world:AddSystems({"sprite", "input", "tree", "tick"})

    local button1 = yecs.EntityFactory:Make("button", {"button1_behavior"})
    local button2 = yecs.EntityFactory:Make("button", {"button2_behavior"})

    local image = yecs.EntityFactory:Make("image", {"image_behavior"})

    local label = yecs.EntityFactory:Make("label")
    local progress = yecs.EntityFactory:Make("progress", {"increase_progress_behavior"})
    local panel = yecs.EntityFactory:Make("panel")
    local palette = yecs.EntityFactory:Make("palette")

    panel:SetSize(320, 128)
    panel.position = {x=0, y=0, z=0}

    progress.position = {x=80, y=8, z=1}
    progress:SetPercent(10)

    image:SetTextureSize(128,128)
    image:SetTexture("data/image/animation/blood/1.png")

    image.position = {x=8, y=yapre.render_height-8-128, z=1}
    image:AddAnimationState("die", "data/image/animation/blood/%d.png", 1, 20)
    image:AddAnimationState("live", "data/image/animation/blood/%d.png", 1, 1)

    label:SetText("O wonder! \nHow many goodly creatures are there here!\nHow beauteous mankind is!\nO brave new world,\nThat has such people in't.")
    label:SetMaxSize(64, -1)
    label.position = {x=8, y=8, z=1}

    button1.position = {x=yapre.render_width-8-32, y=yapre.render_height-8-32, z=1}
    button2.position = {x=yapre.render_width-8-32-32, y=yapre.render_height-8-32, z=1}

    button1:SetState("normal")
    button2:SetState("disabled")

    world:AddEntity(button1)
    world:AddEntity(button2)
    world:AddEntity(label)
    world:AddEntity(image)
    world:AddEntity(progress)
    world:AddEntity(panel)
    world:AddEntity(palette)

    return world 
end

return test_world
