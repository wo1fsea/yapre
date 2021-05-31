local game = {}

local rewind_controller = require("app.rewind_controller")
local world_mario_music = require("app.game.world_mario_music")
local world_slides = require("app.game.world_slides")
local world_label = require("app.game.world_label")
local world_image = require("app.game.world_image")
local world_flappy_duck = require("app.game.world_flappy_duck")

game.worlds = setmetatable({}, {__mode="v"})
game.rewind_controller = nil

function game:Init()
    self.cur_world_idx = 1
    self.worlds = {}
    self.rewind_controller = rewind_controller:Make(self.worlds)

    self.worlds_to_show = {world_flappy_duck, world_label, world_mario_music, world_image}
    
    self.worlds["world_slides"] = world_slides:Make()
    self.worlds[self.cur_world_idx] =  self.worlds_to_show[self.cur_world_idx]:Make()
end

function game:Deinit()
end

function game:NextWorld()
    if self.cur_world_idx == #self.worlds_to_show then return end

    self.worlds[self.cur_world_idx]:Destroy()
    self.cur_world_idx = self.cur_world_idx + 1

    self.worlds[self.cur_world_idx] = self.worlds_to_show[self.cur_world_idx]:Make()
end

function game:PrevWorld()
    if self.cur_world_idx == 1 then return end

    self.worlds[self.cur_world_idx]:Destroy()
    self.cur_world_idx = self.cur_world_idx - 1

    self.worlds[self.cur_world_idx] = self.worlds_to_show[self.cur_world_idx]:Make()
end


function game:Dump()

end

function game:Load()

end


return game
