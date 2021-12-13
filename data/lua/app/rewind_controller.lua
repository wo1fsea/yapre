local yapre = yapre
local rewind_controller = {}

local core = require("core")
local yecs = core.yecs

local serialization = require("core.serialization")

local dump_interval = 1000 -- 1s

rewind_controller.dump_data = {}
rewind_controller.dump_data_cur_idx = 1
rewind_controller.dump_data_max_num = 100

function rewind_controller:OnKey(timestamp, state, multi, keycode)
    if state == 1 then
        keycode = string.char(keycode)
        if keycode == "\t" then
            self:PauseOrResume()
            return
        end

        if not self.paused then
            return
        end

        if keycode == "s" then
            self:Save()
            return
        end

        local delta = 1
        if keycode == "a" then
            delta = -1
        elseif keycode == "d" then
            delta = 1
        else
            return
        end

        local ps = self.progress_selector

        local cur_idx = ps:GetCurrent()
        local max_idx = ps:GetMax()

        cur_idx = cur_idx + delta
        if cur_idx > max_idx then
            cur_idx = max_idx
        elseif cur_idx < 1 then
            cur_idx = 1
        end

        local r_idx = self.dump_data_cur_idx - 1 - ps:GetMax() + cur_idx
        if r_idx < 1 then
            cur_idx = cur_idx - r_idx + 1
            r_idx = 1
        end

        ps:SetCurrent(cur_idx)
        self:Load(r_idx)
    end
end

function rewind_controller:Make(game_worlds)
    local world = yecs.World:New("rewind_controller")
    world:AddSystems({"sprite", "input", "tree", "tick"})
    world.systems["input"].OnKey = function(input_system, timestamp, state, multi, keycode)
        self:OnKey(timestamp, state, multi, keycode)
    end

    local control_panel = yecs.EntityFactory:Make("panel")
    world:AddEntity(control_panel)
    control_panel:SetSize(yapre.render_width, 30)
    control_panel.position.z = 1024

    local progress_selector = yecs.EntityFactory:Make("progress_selector")
    world:AddEntity(progress_selector)
    control_panel.tree:AddChild(progress_selector)

    local label = yecs.EntityFactory:Make("label")
    world:AddEntity(label)
    label.text.size = 2
    label:SetText("PAUSED!")
    label.position.x = 10
    label.position.y = 10
    control_panel.tree:AddChild(label)

    self.game_worlds = game_worlds
    self.progress_selector = progress_selector
    self.control_panel = control_panel
    self.world = world

    self:ShowControlPanel(false)
    self:AddTimer()
    return world
end

function rewind_controller:Dump()
    local game_worlds = self.game_worlds

    local data = {}
    for game_world_key, game_world in pairs(game_worlds) do
        data[game_world_key] = serialization:DumpWorld(game_world)
    end

    local idx = self.dump_data_cur_idx
    idx = idx % self.dump_data_max_num
    self.dump_data[idx] = data
    self.dump_data_cur_idx = self.dump_data_cur_idx + 1
end

function rewind_controller:Load(idx)
    idx = idx % self.dump_data_max_num
    local data = self.dump_data[idx]
    if not data then
        return
    end

    local game_worlds = self.game_worlds
    for game_world_key, game_world in pairs(game_worlds) do
        game_world:Destroy()
    end

    for game_world_key, game_world_data in pairs(data) do
        local game_world = serialization:LoadWorld(game_world_data)
        game_world:Pause()
        game_worlds[game_world_key] = game_world
    end
end

function rewind_controller:PauseOrResume()
    local game_worlds = self.game_worlds
    if self.paused then
        self.paused = false
        for _, game_world in pairs(game_worlds) do
            game_world:Resume()
        end
        self.dump_data_cur_idx = self.dump_data_cur_idx - 1
        self.progress_selector:SetCurrent(-1)
        self:ShowControlPanel(false)
    else
        self.paused = true
        for _, game_world in pairs(game_worlds) do
            game_world:Pause()
        end
        self:Dump()
        self.progress_selector:SetCurrent(self.progress_selector:GetMax())
        self:ShowControlPanel(true)
    end
end

function rewind_controller:ShowControlPanel(v)
    if v then
        self.control_panel.position.y = 0
    else
        self.control_panel.position.y = -yapre.render_height
    end
end

function rewind_controller:AddTimer()
    yapre.AddTimer(dump_interval, function()
        if not self.paused then
            self:Dump()
        end
        self:AddTimer()
    end)
end

function rewind_controller:Save()
    if not self.paused then
        return
    end
    local data = self.dump_data[self.dump_data_cur_idx - 1]
    data = serialization:DumpWorld(self.world)
    table.save(data, "./dump.lua")
end

return rewind_controller
