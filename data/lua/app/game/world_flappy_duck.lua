local world_flappy_duck = {}

local core = require("core")
local yecs = core.yecs

yecs.Behavior:Register("world_flappy_duck_duck_behavior", {
    OnInit=function(self)
        self.tags["duck"] = true
        
        self.size = {width = 32, height=32}
        self:SetTextureSize(self.size.width, self.size.height)
        self:SetTexture("./image/flappy_duck/duck/up1.png")

        self:AddAnimationState("up", "./image/flappy_duck/duck/up%d.png", 1, 2, true)
        self:AddAnimationState("down", "./image/flappy_duck/duck/down%d.png", 1, 2, true)
        self:PlayAnimation("up")
        self:SetBorderEnabled(false)
        
        self:Born()
        self.data.y = yapre.render_height * 2
        self.position.y = self.data.y
        self:Die()

        self.tick:AddTick("duck_tick", 
        function(delta_ms)
            local cs = self.world:GetEntitiesByTags({"cactus"})
            for _, c in pairs(cs) do
                if self.position.x + self.size.width - 12 < c.position.x or
                    self.position.x + 12 > c.position.x + c.size.width or
                    self.position.y + self.size.height - 12 < c.position.y or
                    self.position.y + 12 > c.position.y + c.size.height then
                    -- nothing
                else
                    self:Die()
                end
            end

            if self.position.y > yapre.render_height or self.position.y < - self.size.height then
                self:Die()
            end

            if self.position.y > yapre.render_height * 2 and self.data.up_speed < 0 then
                return 
            end 

           if delta_ms > 100 then delta_ms = 100 end

           local last_speed = self.data.up_speed
           self.data.y = self.data.y - delta_ms * self.data.up_speed / 1000
           self.position.y = self.data.y // 1
           self.data.up_speed = self.data.up_speed - self.data.g * delta_ms / 1000
           if last_speed * self.data.up_speed > 0 then return end
           if self.data.up_speed > 0 then
                self:PlayAnimation("up")
           else
                self:PlayAnimation("down")
           end
       end
       )
   end,
   Die=function(self)
       self.data.dead = true
       self.data.g = 1000
       if self.data.up_speed > 0 then self.data.up_speed = 0 end

   end,
   Born=function(self)
       self.data.dead = false

       self.data.up_speed = 100
       self.data.d_up_speed = 150 
       self.data.y = 120
       self.data.g = 500

       self.position.x = 32
       self.position.y = self.data.y
   end,
   Fly=function(self)
       if self.data.dead then return end
       local last_speed = self.data.up_speed
       self.data.up_speed = self.data.d_up_speed 
       if last_speed <= 0 then 
           self:PlayAnimation("up")
       end
   end
})

yecs.EntityFactory:Register(
"flappy_duck",
{"position", "sprite", "tree", "size", "animation", "input", "tags", "data", "tick"},
{"image", "world_flappy_duck_duck_behavior"})


yecs.Behavior:Register("world_flappy_duck_cactus_behavior", {
    OnInit=function(self)
        self.tags["cactus"] = true
        self.size = {width=9, height=240}
        for idx = 1, 30, 1 do
            self.sprite:AddSprite(
            "cactus_"..idx, 
            "./image/flappy_duck/cactus.png", 
            {
                size={width=8, height=8}, 
                offset={x=0, y=(idx-1)*8, z=1}
            })
        end 
        self.data.x = -10
        self.data.y = 0
        self.position.z = 64

        self.tick:AddTick("cactus_tick", 
        function(delta_ms)
            local duck = self.world:GetEntityByTags({"duck"})
            if duck.data.dead then
                self.data.x = -self.size.width
                self.position.x = self.data.x
                return
            end
            
            self.data.x = self.data.x - 10 / delta_ms

            if self.data.x < -self.size.width then
                local cs = self.world:GetEntitiesByTags({"cactus"})
                local p_x = yapre.render_width
                for _, c in pairs(cs) do
                    if c.position.x + 80 > p_x then
                        p_x = c.position.x + 80
                    end
                end

                self.data.x = p_x
                local y = 50 + 140 * math.random() // 1
                if math.random() > 0.5 then
                    self.data.y = y - self.size.height
                else
                    self.data.y = y
                end
            end

            self.position.x = self.data.x // 1
            self.position.y = self.data.y //1
        end
        )
    end
})

yecs.Behavior:Register("world_flappy_duck_label_behavior", {
    OnInit=function(self)
        self:AddComponent("tags")
        self:AddComponent("tick")
        self.tags["label"] = true

        self:SetText("TAP THE BUTTON \nTO START!")
        self:SetFontSize(2)
        self.position = {x=128, y=32, z=1}
        self.position.z = 64

        self.tick:AddTick("label_tick", 
        function(delta_ms)
            local duck = self.world:GetEntityByTags({"duck"})
            if duck.data.dead and duck.position.y < yapre.render_height * 2 then
                self:SetText("YOU DIED!\nTAP THE BUTTON \nTO START!")
            elseif not duck.data.dead then
                self:SetText("")
            end
        end
        )
    end,
})

yecs.Behavior:Register("world_flappy_duck_duck_head_behavior", {
    OnInit=function(self)
        self:AddComponent("tags")
        self:AddComponent("tick")
        self.tags["duck_head"] = true

        self.position = {x=16, y=yapre.render_height-128, z=10}
        self:SetTextureSize(128,128)
        self:SetTexture("./image/flappy_duck/duck_head.png")

        self.tick:AddTick("label_tick", 
        function(delta_ms)
            local duck = self.world:GetEntityByTags({"duck"})
            if duck.data.dead and duck.position.y < yapre.render_height * 2 then
                self.position.x = 16 
            elseif not duck.data.dead then
                self.position.x = -1024 
            end
        end
        )
    end,
})

yecs.EntityFactory:Register(
"cactus",
{"position", "sprite", "tree", "size", "tick", "tags", "data"},
{"world_flappy_duck_cactus_behavior"})

yecs.Behavior:Register("world_flappy_duck_button_behavior", {
    OnClicked=function(self, x, y)
        local duck = self.world:GetEntityByTags({"duck"})
        if duck.data.dead then
            if duck.position.y > yapre.render_height then
                duck:Born()
            end
            return
        end
        duck:Fly()
        
    end,
    OnInit=function(self)
        self:AddComponent("tags")
        self.tags["button"] = true
        self:SetButtonSize(64, 64)
        self.position.y = yapre.render_height - 64 - 32 
        self.position.x = yapre.render_width - 64 - 32
    end
})

function world_flappy_duck:Make()
    local world = yecs.World:New("world_flappy_duck")
    world:AddSystems({"sprite", "input", "tree", "tick"})
    
    local button = yecs.EntityFactory:Make("button", {"world_flappy_duck_button_behavior"})    
    local label = yecs.EntityFactory:Make("label", {"world_flappy_duck_label_behavior"})
    local duck_head = yecs.EntityFactory:Make("image", {"world_flappy_duck_duck_head_behavior"})

    local duck = yecs.EntityFactory:Make("flappy_duck")
    local cactus1 = yecs.EntityFactory:Make("cactus")
    local cactus2 = yecs.EntityFactory:Make("cactus")
    local cactus3 = yecs.EntityFactory:Make("cactus")
    local cactus4 = yecs.EntityFactory:Make("cactus")
    local cactus5 = yecs.EntityFactory:Make("cactus")

    local background = yecs.Entity:New({"sprite", "position"}, {})

    local background_stars = yecs.EntityFactory:Make("panel")
    local background_stars1 = yecs.EntityFactory:Make("image")
    local background_stars2 = yecs.EntityFactory:Make("image")

    local background_layer1 = yecs.EntityFactory:Make("image")
    local background_layer2 = yecs.EntityFactory:Make("image")

    background_layer1:SetTexture( "./image/flappy_duck/skybg1.png")
    background_layer1:SetTextureSize(128, 128)
    background_layer1:SetBorderEnabled(false)
    background_layer1.position.y = 180
    
    background_layer2:SetTexture( "./image/flappy_duck/skybg2.png")
    background_layer2:SetTextureSize(128, 128)
    background_layer2:SetBorderEnabled(false)
    background_layer2.position.y = 200

    background_stars:SetSize(0, 0)
    background_stars.tree:AddChild(background_stars1)
    background_stars.tree:AddChild(background_stars2)

    background_stars1:SetTexture( "./image/flappy_duck/stars.png")
    background_stars1:SetTextureSize(128, 128)
    background_stars1:SetBorderEnabled(false)
    background_stars1.position.x = 16
    background_stars1.position.y = 16

    background_stars2:SetTexture( "./image/flappy_duck/stars.png")
    background_stars2:SetTextureSize(128, 128)
    background_stars2:SetBorderEnabled(false)
    background_stars2.position.x = 16 * 2 + 128
    background_stars2.position.y = 32

    background.sprite:AddSprite(
    "background", 
    "./image/ui/blank2.png", 
    {size={width=yapre.render_width, height=yapre.render_height}, color=yapre.palette.colors[1]} 
    )

    background.position.z = 0
    background_stars.position.z = 2
    background_layer1.position.z = 1
    background_layer2.position.z = 3
    duck.position.z = 128
    button.position.z = 128

    world:AddEntity(button)
    world:AddEntity(label)
    world:AddEntity(duck)
    world:AddEntity(duck_head)
    world:AddEntity(background)
    world:AddEntity(background_stars)
    world:AddEntity(background_stars1)
    world:AddEntity(background_stars2)
    world:AddEntity(background_layer1)
    world:AddEntity(background_layer2)
    world:AddEntity(cactus1)
    world:AddEntity(cactus2)
    world:AddEntity(cactus3)
    world:AddEntity(cactus4)
    return world
end

return world_flappy_duck
