local yapre = yapre
local world_label = {}

local core = require("core")
local yecs = core.yecs

local panel_width = 320
local panel_height = 240

yecs.Behavior:Register("world_label_time_label_behavior", {
    OnInit = function(self)
        self:AddComponent("tick")
        self:AddComponent("data")
        self.data.now_time = 0
        local interval = 23
        self.data.timer_callback = function(delta_ms)
            self.data.now_time = self.data.now_time + interval + delta_ms
            self:SetText(string.format("timing: %.4f", self.data.now_time / 1000))
            self.tick:AddTimer("time_tick", interval, self.data.timer_callback)
        end
        self.tick:AddTimer("time_tick", interval, self.data.timer_callback)
    end
})

function world_label:Make()
    local world = yecs.World:New("world_label")
    world:AddSystemsByKeys({"sprite", "input", "tree", "tick", "layout"})
    
    local canvas = yecs.EntityFactory:Make("panel")
    world:AddEntity(canvas)
    canvas:SetSize(panel_width, panel_height)

    canvas.layout:SetCenterX(world:GetRoot().layout:GetCenterX(), 0)
    canvas.layout:SetCenterY(world:GetRoot().layout:GetCenterY(), 0)

    local time_label = yecs.EntityFactory:Make("label", {"world_label_time_label_behavior"})
    local world_label_a = yecs.EntityFactory:Make("label")
    local world_label_b = yecs.EntityFactory:Make("label")
    local world_label_c = yecs.EntityFactory:Make("label")
    local world_label_d = yecs.EntityFactory:Make("label")

    time_label:SetFontSize(2)
    time_label.position = {
        x = 8,
        y = panel_height - 16 - 4,
        z = 1
    }

    local worlds =
        "O wonder! \nHow many goodly creatures are there here!\nHow beauteous mankind is!\nO brave new world,\nThat has such people in't."
    world_label_a:SetText(worlds)
    world_label_b:SetText(worlds)
    world_label_c:SetText(worlds)
    world_label_d:SetText("Label test")

    world_label_a:SetMaxSize(32, -1)
    world_label_a.position = {
        x = 8,
        y = 8,
        z = 1
    }

    world_label_b:SetMaxSize(panel_width - 48 - 8, -1)
    world_label_b.position = {
        x = 48,
        y = 8,
        z = 1
    }

    world_label_c:SetMaxSize(panel_width - 48 - 8, -1)
    world_label_c.position = {
        x = 48,
        y = 9 * 4 + 8 + 8,
        z = 1
    }
    world_label_c:SetFontSize(2)

    world_label_d:SetMaxSize(panel_width - 48 - 8, -1)
    world_label_d.position = {
        x = 48,
        y = panel_height - 56,
        z = 1
    }
    world_label_d:SetFontSize(4)

    canvas.tree:AddChild(time_label)
    canvas.tree:AddChild(world_label_a)
    canvas.tree:AddChild(world_label_b)
    canvas.tree:AddChild(world_label_c)
    canvas.tree:AddChild(world_label_d)

    return world
end

return world_label
