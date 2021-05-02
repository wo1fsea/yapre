
local core = require("core")
local yecs = core.yecs

local app = {}


function app:Init()
    yapre.SetRenderSize(320, 240)

    self.world1 = yecs.World:New("world1")
    self.world1:AddSystems({"sprite", "input", "tree", "tick"})

    self.button1 = yecs.EntityFactory:Make("button")
    self.button2 = yecs.EntityFactory:Make("button")

    self.label = yecs.EntityFactory:Make("label")
    self.image = yecs.EntityFactory:Make("image")
    self.progress = yecs.EntityFactory:Make("progress")
    self.panel = yecs.EntityFactory:Make("panel")
    self.palette = yecs.EntityFactory:Make("palette")
    
    self.panel:SetSize(320, 128)
    self.panel.position = {x=0, y=0, z=0}

    self.progress.position = {x=80, y=8, z=1}
    self.progress:SetPercent(10)
    
    self.image:SetTextureSize(128,128)
    self.image:SetTexture("data/image/animation/blood/1.png")

    self.image.position = {x=8, y=240-8-128, z=1}
    self.image:AddAnimationState("die", "data/image/animation/blood/%d.png", 1, 20)
    self.image:AddAnimationState("live", "data/image/animation/blood/%d.png", 1, 1)

    self.label:SetText("O wonder! \nHow many goodly creatures are there here!\nHow beauteous mankind is!\nO brave new world,\nThat has such people in't.")
    self.label:SetMaxSize(64, -1)
    self.label.position = {x=8, y=8, z=1}

    function self.button1.OnClicked(_, x, y)
        self.image:PlayAnimation("die")
        self.button1:SetState("disabled")
        self.button2:SetState("normal")
    end
    self.button1.position = {x=320-8-32, y=240-8-32, z=1}
    
    function self.button2.OnClicked(_, x, y)
        self.image:PlayAnimation("live")
        self.button2:SetState("disabled")
        self.button1:SetState("normal")
    end
    self.button2.position = {x=320-8-32-32, y=240-8-32, z=1}
    
    self.button1:SetState("normal")
    self.button2:SetState("disabled")

    self.world1:AddEntity(self.button1)
    self.world1:AddEntity(self.button2)
    self.world1:AddEntity(self.label)
    self.world1:AddEntity(self.image)
    self.world1:AddEntity(self.progress)
    self.world1:AddEntity(self.panel)
    self.world1:AddEntity(self.palette)

    return true
end

local p = 0
local delta_i = 1
function app:Update(delta_ms)
    self.world1:Update(delta_ms)
    if self.world1.paused then return end
    p = p + delta_i 
    if p == 100 or p == 0 then delta_i = -1 * delta_i end
    self.progress:SetPercent(p)
end

function app:Deinit()
end

function app:Reload()

end

return app
