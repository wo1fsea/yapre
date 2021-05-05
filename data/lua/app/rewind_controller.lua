local rewind_controller = {}

local core = require("core")
local yecs = core.yecs

local s = require("core.serialization")

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
        
        if not self.game_world.paused then return end
        
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

function rewind_controller:Make(game_world)
    local world = yecs.World:New("rewind_controller")
    world:AddSystems({"sprite", "input", "tree", "tick"})
    world.systems["input"].OnKey = function(input_system, timestamp, state, multi, keycode)
        self:OnKey(timestamp, state, multi, keycode)
    end

    local progress_selector = yecs.EntityFactory:Make("progress_selector")
    world:AddEntity(progress_selector)
    progress_selector.position.z = 1024
    progress_selector.position.y = -10
    
    self.game_world = game_world
    self.progress_selector = progress_selector
    
    self:AddTimer()
    return world
end

function rewind_controller:Dump()
    if self.game_world.paused then return end

    local data = s:DumpWorld(self.game_world)
    local idx = self.dump_data_cur_idx
    idx = idx % self.dump_data_max_num
    self.dump_data[idx] = data
    self.dump_data_cur_idx = self.dump_data_cur_idx + 1
end

function rewind_controller:Load(idx)
    idx = idx % self.dump_data_max_num
    local data = self.dump_data[idx]
    if not data then return end 
    
    if self.game_world then
        self.game_world:Destroy()
        self.game_world = nil
    end
    
    self.game_world = s:LoadWorld(data)
    self.game_world:Pause()
end

function rewind_controller:PauseOrResume()
    local game_world = self.game_world
    if game_world.paused then
        game_world:Resume()
        self.dump_data_cur_idx = self.dump_data_cur_idx - 1
        self.progress_selector:SetCurrent(-1)
        self.progress_selector.position.y = -10
    else
        self:Dump()
        game_world:Pause()
        self.progress_selector:SetCurrent(self.progress_selector:GetMax())
        self.progress_selector.position.y = 0
    end
end

function rewind_controller:AddTimer()
    yapre.AddTimer(250, function()
        self:Dump()
        self:AddTimer()
    end)
end


return rewind_controller
