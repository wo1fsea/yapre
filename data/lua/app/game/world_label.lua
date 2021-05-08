local test_label = {}

local core = require("core")
local yecs = core.yecs

yecs.Behavior:Register("test_label_time_label_behavior", {
    OnInit=function(self)
        self:AddComponent("tick")
        self:AddComponent("data")
        self.data.now_time = 0
        local interval = 23
        self.data.timer_callback = function(delta_ms)
            self.data.now_time = self.data.now_time + interval + delta_ms
            self:SetText(string.format("timing: %.4f", self.data.now_time/1000))
            self.tick:AddTimer("time_tick", interval, self.data.timer_callback)
        end
        self.tick:AddTimer("time_tick", interval, self.data.timer_callback)
    end,
})


function test_label:Make()
    local world = yecs.World:New("test_label")
    world:AddSystems({"sprite", "input", "tree", "tick"})

    local time_label = yecs.EntityFactory:Make("label", {"test_label_time_label_behavior"})
    local world_label_a = yecs.EntityFactory:Make("label")
    local world_label_b = yecs.EntityFactory:Make("label")
    local world_label_c = yecs.EntityFactory:Make("label")

    time_label:SetFontSize(2)
    time_label.position = {x=8, y=yapre.render_height-16-4, z=1}
    
    local worlds = "O wonder! \nHow many goodly creatures are there here!\nHow beauteous mankind is!\nO brave new world,\nThat has such people in't."
    world_label_a:SetText(worlds)
    world_label_b:SetText(worlds)
    world_label_c:SetText(worlds)

    world_label_a:SetMaxSize(32, -1)
    world_label_a.position = {x=8, y=8, z=1}

    world_label_b:SetMaxSize(yapre.render_width-48-8, -1)
    world_label_b.position = {x=48, y=8, z=1}

    world_label_c:SetMaxSize(yapre.render_width-48-8, -1)
    world_label_c.position = {x=48, y=9*4+8+8, z=1}
    world_label_c:SetFontSize(2)

    world:AddEntity(time_label)
    world:AddEntity(world_label_a)
    world:AddEntity(world_label_b)
    world:AddEntity(world_label_c)

    return world
end

return test_label
