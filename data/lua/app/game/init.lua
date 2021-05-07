local game = {}

local rewind_controller = require("app.rewind_controller")
local world_mario_music = require("app.game.world_mario_music")

game.worlds = setmetatable({}, {__mode="v"})
game.rewind_controller = nil

function game:Init()
    self.rewind_controller = rewind_controller:Make(self.worlds)
    self.worlds["world_mario_music"] = world_mario_music:Make()
end

function game:Deinit()
end

function game:NextWorld()
end

function game:PrevWorld()
end


function game:Dump()

end

function game:Load()

end


return game
