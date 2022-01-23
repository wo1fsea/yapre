local yapre = yapre
local world_flappy_duck = {}
local core = require("core")
local yecs = core.yecs

local panel_width = 320
local panel_height = 240

yecs.Behavior:Register("world_flappy_duck_duck_behavior", {
    OnInit = function(self)
        self.tags["duck"] = true

        self.size = {
            width = 32,
            height = 32
        }
        self:SetTextureSize(self.size.width, self.size.height)
        self:SetTexture("./image/flappy_duck/duck/up1.png")

        self:AddAnimationState("up", "./image/flappy_duck/duck/up%d.png", 1, 2, true)
        self:AddAnimationState("down", "./image/flappy_duck/duck/down%d.png", 1, 2, true)
        self:PlayAnimation("up")
        self:SetBorderEnabled(false)

        self:Born()
        self.data.y = panel_height * 2
        self.position.y = self.data.y
        self.position.z = 128
        self:Die()

        self.tick:AddTick("duck_tick", function(delta_ms)
            local cs = self.world:GetEntitiesByTags({"cactus"})
            for _, c in pairs(cs) do
                if self.position.x + self.size.width - 12 < c.position.x or self.position.x + 12 > c.position.x +
                    c.size.width or self.position.y + self.size.height - 12 < c.position.y or self.position.y + 12 >
                    c.position.y + c.size.height then
                    -- nothing
                else
                    self:Die()
                end
            end

            if self.position.y > panel_height or self.position.y < -self.size.height then
                self:Die()
            end

            if self.position.y > panel_height * 2 and self.data.up_speed < 0 then
                return
            end

            if delta_ms > 100 then
                delta_ms = 100
            end

            local last_speed = self.data.up_speed
            self.data.y = self.data.y - delta_ms * self.data.up_speed / 1000
            self.position.y = self.data.y // 1
            self.data.up_speed = self.data.up_speed - self.data.g * delta_ms / 1000
            if last_speed * self.data.up_speed > 0 then
                return
            end
            if self.data.up_speed > 0 then
                self:PlayAnimation("up")
            else
                self:PlayAnimation("down")
            end
        end)
    end,
    Die = function(self)
        self.data.dead = true
        self.data.g = 1000
        if self.data.up_speed > 0 then
            self.data.up_speed = 0
        end

    end,
    Born = function(self)
        self.data.dead = false

        self.data.up_speed = 100
        self.data.d_up_speed = 150
        self.data.y = panel_height / 2
        self.data.g = 500

        self.position.x = 32
        self.position.y = self.data.y
    end,
    Fly = function(self)
        if self.data.dead then
            return
        end
        local last_speed = self.data.up_speed
        self.data.up_speed = self.data.d_up_speed
        if last_speed <= 0 then
            self:PlayAnimation("up")
        end
    end
})

yecs.EntityFactory:Register("flappy_duck",
    {"position", "sprite", "tree", "size", "animation", "input", "tags", "data", "tick"},
    {"image", "world_flappy_duck_duck_behavior"})

yecs.Behavior:Register("world_flappy_duck_cactus_behavior", {
    OnInit = function(self)
        self.tags["cactus"] = true
        self.size = {
            width = 9,
            height = 240
        }
        for idx = 1, 30, 1 do
            self.sprite:AddSprite("cactus_" .. idx, "./image/flappy_duck/cactus.png", {
                size = {
                    width = 8,
                    height = 8
                },
                offset = {
                    x = 0,
                    y = (idx - 1) * 8,
                    z = 1
                }
            })
        end
        self.data.x = -10
        self.data.y = 0
        self.data.speed = 10
        self.position.z = 64

        self.tick:AddTick("cactus_tick", function(delta_ms)
            local duck = self.world:GetEntityByTags({"duck"})
            if duck.data.dead then
                self.data.x = -self.size.width
                self.position.x = self.data.x
                return
            end

            self.data.x = self.data.x - self.data.speed / delta_ms

            if self.data.x < -self.size.width then
                local cs = self.world:GetEntitiesByTags({"cactus"})
                local p_x = panel_width
                for _, c in pairs(cs) do
                    if c.data.x + 80 > p_x then
                        p_x = c.data.x + 80
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
            self.position.y = self.data.y // 1
        end)
    end
})

yecs.Behavior:Register("world_flappy_duck_label_behavior", {
    OnInit = function(self)
        self:AddComponent("tags")
        self:AddComponent("tick")
        self.tags["label"] = true

        self:SetText("TAP THE BUTTON \nTO START!")
        self:SetFontSize(2)
        self.position = {
            x = 128,
            y = 32,
            z = 1
        }
        self.position.z = 64

        self.tick:AddTick("label_tick", function(delta_ms)
            local duck = self.world:GetEntityByTags({"duck"})
            if duck.data.dead and duck.position.y < panel_height * 2 then
                self:SetText("YOU DIED!\nTAP THE BUTTON \nTO START!")
            elseif not duck.data.dead then
                self:SetText("")
            end
        end)
    end
})

yecs.Behavior:Register("world_flappy_duck_duck_head_behavior", {
    OnInit = function(self)
        self:AddComponent("tags")
        self:AddComponent("tick")
        self.tags["duck_head"] = true

        self.position = {
            x = 16,
            y = panel_height - 128,
            z = 64
        }
        self:SetTextureSize(128, 128)
        self:SetTexture("./image/flappy_duck/duck_head.png")
        self:SetBorderEnabled(false)

        self.tick:AddTick("label_tick", function(delta_ms)
            local duck = self.world:GetEntityByTags({"duck"})
            if duck.data.dead and duck.position.y < panel_height * 2 then
                self.position.x = 16
            elseif not duck.data.dead then
                self.position.x = -1024
            end
        end)
    end
})

yecs.EntityFactory:Register("cactus", {"position", "sprite", "tree", "size", "tick", "tags", "data"},
    {"world_flappy_duck_cactus_behavior"})

yecs.Behavior:Register("world_flappy_duck_button_behavior", {
    OnClicked = function(self, x, y)
        local duck = self.world:GetEntityByTags({"duck"})
        if duck.data.dead then
            if duck.position.y > panel_height then
                duck:Born()
            end
            return
        end
        duck:Fly()
    end,
    OnInit = function(self)
        self:AddComponent("tags")
        self.tags["button"] = true
        self:SetButtonSize(64, 64)
        self.position.y = panel_height - 64 - 32
        self.position.x = panel_width - 64 - 32
        self.position.z = 128
        self:BindKey(" ")
    end
})

yecs.Behavior:Register("world_flappy_duck_background_behavior", {
    OnInit = function(self)
        self:AddComponent("tags")
        self:AddComponent("tick")
        self:AddComponent("data")

        self.tags["background"] = true
        self.data.tag = "background"

        self.size = {
            width = 128,
            height = 128
        }
        self:SetTextureSize(128, 128)
        self:SetBorderEnabled(false)

        self.data.x = 0
        self.data.y = 0
        self.data.z = 0
        self.data.speed = 10
        self.data.interval = 0
        self.data.y_rate = 0

        self.tick:AddTick("background_tick", function(delta_ms)
            local duck = self.world:GetEntityByTags({"duck"})
            if duck.data.dead then
                return
            end

            self.data.x = self.data.x - self.data.speed / delta_ms
            if self.data.x < -self.size.width then
                local bs = self.world:GetEntitiesByTags({self.data.tag})
                local p_x = 0 -- panel_width
                for _, b in pairs(bs) do
                    if b.data.x + self.data.interval + self.size.width > p_x then
                        p_x = b.data.x + self.data.interval + self.size.width
                    end
                end
                self.data.x = p_x
            end

            self.position.x = self.data.x // 1
            self.position.y = (self.data.y + self.data.y_rate * (duck.data.y - panel_height / 2)) // 1
            self.position.z = self.data.z // 1

        end)
    end
})

function world_flappy_duck:MakeBackground(canvas)

    local background = yecs.Entity:New({"sprite", "position"}, {})
    background.sprite:AddSprite("background", "./image/ui/blank2.png", {
        size = {
            width = panel_width,
            height = panel_height
        },
        color = yapre.palette.colors[1]
    })

    background.position.z = 1
    canvas.tree:AddChild(background)

    local idx = 1
    for idx = 1, 3, 1 do
        local background_stars = yecs.EntityFactory:Make("image", {"world_flappy_duck_background_behavior"})
        canvas.tree:AddChild(background_stars)

        background_stars:SetTexture("./image/flappy_duck/stars.png")
        background_stars.data.tag = "background_stars"
        background_stars.tags[background_stars.data.tag] = true
        background_stars.data.x = 16 * (idx - 1) + 128 * (idx - 1)
        background_stars.data.y = math.random(32)
        background_stars.data.z = 3
        background_stars.data.interval = 32
        background_stars.data.speed = 2
        background_stars.data.y_rate = 0.03

        background_stars.position.x = background_stars.data.x
        background_stars.position.y = background_stars.data.y
        background_stars.position.z = background_stars.data.z
    end

    for idx = 1, 4, 1 do
        local background_layer = yecs.EntityFactory:Make("image", {"world_flappy_duck_background_behavior"})
        canvas.tree:AddChild(background_layer)

        background_layer:SetTexture("./image/flappy_duck/skybg1.png")
        background_layer.data.tag = "background_layer1"
        background_layer.tags[background_layer.data.tag] = true
        background_layer.data.x = 127 * (idx - 1)
        background_layer.data.y = 160
        background_layer.data.z = 2
        background_layer.data.speed = 8
        background_layer.data.interval = -1
        background_layer.data.y_rate = -0.1

        background_layer.position.x = background_layer.data.x
        background_layer.position.y = background_layer.data.y
        background_layer.position.z = background_layer.data.z
    end

    for idx = 1, 4, 1 do
        local background_layer = yecs.EntityFactory:Make("image", {"world_flappy_duck_background_behavior"})
        canvas.tree:AddChild(background_layer)

        background_layer:SetTexture("./image/flappy_duck/skybg2.png")
        background_layer.data.tag = "background_layer2"
        background_layer.tags[background_layer.data.tag] = true
        background_layer.data.x = 127 * (idx - 1)
        background_layer.data.y = 200
        background_layer.data.z = 4
        background_layer.data.interval = -1
        background_layer.data.y_rate = -0.2

        background_layer.position.x = background_layer.data.x
        background_layer.position.y = background_layer.data.y
        background_layer.position.z = background_layer.data.z
    end
end

function world_flappy_duck:Make()
    local world = yecs.World:New("world_flappy_duck")
    world:AddSystemsByKeys({"sprite", "input", "tree", "tick", "layout"})
    local canvas = yecs.EntityFactory:Make("panel")
    world:AddEntity(canvas)
    canvas:SetSize(panel_width, panel_height)
    canvas.layout:SetCenterX(world:GetRoot().layout:GetCenterX(), 0)
    canvas.layout:SetCenterY(world:GetRoot().layout:GetCenterY(), 0)

    local button = yecs.EntityFactory:Make("button", {"world_flappy_duck_button_behavior"})
    canvas.tree:AddChild(button)

    local label = yecs.EntityFactory:Make("label", {"world_flappy_duck_label_behavior"})
    canvas.tree:AddChild(label)

    local duck_head = yecs.EntityFactory:Make("image", {"world_flappy_duck_duck_head_behavior"})
    canvas.tree:AddChild(duck_head)

    local duck = yecs.EntityFactory:Make("flappy_duck")
    canvas.tree:AddChild(duck)

    for _ = 1, 5, 1 do
        local cactus = yecs.EntityFactory:Make("cactus")
        canvas.tree:AddChild(cactus)
    end

    self:MakeBackground(canvas)
    return world
end

return world_flappy_duck
