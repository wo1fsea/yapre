local game = {}

local rewind_controller = require("app.rewind_controller")
local world_mario_music = require("app.game.world_mario_music")
local world_slides = require("app.game.world_slides")
local world_label = require("app.game.world_label")
local world_image = require("app.game.world_image")
local world_flappy_duck = require("app.game.world_flappy_duck")
local world_word_slide_maker = require("app.game.world_word_slide_maker")
local world_palette = require("app.game.world_palette")

game.worlds = setmetatable({}, {__mode="v"})
game.rewind_controller = nil

function game:Init()
    self.cur_world_idx = 1
    self.worlds = {}
    self.rewind_controller = rewind_controller:Make(self.worlds)

    self.worlds_to_show = {
        world_word_slide_maker:Make("Game &&\nHomebrew Pixel Game\n", 3),
        world_word_slide_maker:Make("1.IDEA\n", 3),
        world_word_slide_maker:Make("300 Mechanics\nhttps://www.squidi.net/three/\n", 2),
        world_word_slide_maker:Make("2.PROGRAM\n", 3),
        world_word_slide_maker:Make("A GAME LOOP\n\na) input\nb) simulation\nc) output\n", 2),
        world_word_slide_maker:Make("input\n\nkeyboard & mouse\ntouch\n", 2),
        world_word_slide_maker:Make("API\n\nBindKeyboardInputCallback\nBindMouseInputCallback\nBindTouchInputCallback\n", 2),
        world_word_slide_maker:Make("output\n\na) video\nb) audio\n", 2),
        world_word_slide_maker:Make("API -- Video\n\nDrawSprite\n", 2),
        world_image,
        world_word_slide_maker:Make("API -- Audio\n\nBeep\n", 2),
        world_mario_music,
        world_word_slide_maker:Make("simulation\n\nscripting\n", 2),
        world_word_slide_maker:Make("scripting language\n\nPython (?)\nlua (v)\n", 2),
        world_word_slide_maker:Make("tools\n\nBuild\n  - CMake\nCI\n  - Github Actions (v)\n  - Travis CI (x)\n", 2),
        world_word_slide_maker:Make("Amway Style Recommendation\n\nVSCode + vcpkg + clang + CMake\nvim + CMake\n", 2),
        world_word_slide_maker:Make("3.ASSET\n", 3),
        world_word_slide_maker:Make("pixel art & why\n", 2),
        world_word_slide_maker:Make("howto\n", 2),
        world_word_slide_maker:Make("tools\n\naseprite\n", 2),
        world_word_slide_maker:Make("tutorial\n\nhttps://saint11.org/blog/\npixel-art-tutorials/\n", 2),
        world_flappy_duck,
        world_word_slide_maker:Make("palette\n", 2),
        world_palette,
        world_word_slide_maker:Make("PM\n\na) #1GAM (?)\nb) 15min task\n", 2),
        world_word_slide_maker:Make("many thinks\n", 2),
        -- world_label,
    }
    
    self.worlds["world_slides"] = world_slides:Make()
    self.worlds[self.cur_world_idx] = self.worlds_to_show[self.cur_world_idx]:Make()
end

function game:Deinit()
end

function game:NextWorld()
    if self.cur_world_idx == #self.worlds_to_show then
        return
    end

    self.worlds[self.cur_world_idx]:Destroy()
    self.worlds[self.cur_world_idx] = nil
    self.cur_world_idx = self.cur_world_idx + 1

    self.worlds[self.cur_world_idx] = self.worlds_to_show[self.cur_world_idx]:Make()
end

function game:PrevWorld()
    if self.cur_world_idx == 1 then
        return
    end

    self.worlds[self.cur_world_idx]:Destroy()
    self.worlds[self.cur_world_idx] = nil
    self.cur_world_idx = self.cur_world_idx - 1

    self.worlds[self.cur_world_idx] = self.worlds_to_show[self.cur_world_idx]:Make()
end

function game:Dump()

end

function game:Load()

end

function game:Update(delta_ms)
    for idx = 1, #self.worlds_to_show, 1 do
        if self.worlds[idx] then
            self.cur_world_idx = idx
        end
    end
end

return game
