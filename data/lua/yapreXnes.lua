package.path = package.path .. ";./lua/?.lua;./lua/?/init.lua"

local yapre = yapre

yapre.log = require("log")
yapre.dbg = require("debugger")
yapre.dbg.read = yapre.DebugRead
yapre.dbg.write = yapre.DebugWrite

require("strict")
require("table_save")

local palette_data = require("core.data.palette_data")
yapre.palette = palette_data

local background_color = palette_data.background_color
yapre.SetClearColor(background_color.r, background_color.g, background_color.b, background_color.a)

local NES = require("nes.nes")
local PPU = require("nes.ppu")
local PADS = require("nes.pads")

local file = "./roms/Super Mario Bros (E).nes"
local loglvl = 0
local Nes = NES:new({
    file = file,
    loglevel = loglvl
    --[[
    pc = false,
    palette = UTILS.map(
    PALETTE:defacto_palette(),
    function(c)
    return c
    end
    )
    --]]
})
local width = 256
local height = 240
local pixSize = 1
local sound = false
local DEBUG = false

local time = 0
local timeTwo = 0
local rate = 1 / 59.94
local rate = 1 / 59.94
local fps = 0
local tickRate = 0
local tickRatetmp = 0
local pixelCount = PPU.SCREEN_HEIGHT * PPU.SCREEN_WIDTH
local Pad = PADS.Pad

local keyEvents = {}
local keyButtons = {
    ["w"] = Pad.UP,
    [119] = Pad.UP,
    ["a"] = Pad.LEFT,
    [97] = Pad.LEFT,
    ["s"] = Pad.DOWN,
    [115] = Pad.DOWN,
    ["d"] = Pad.RIGHT,
    [100] = Pad.RIGHT,
    ["o"] = Pad.A,
    [111] = Pad.A,
    ["p"] = Pad.B,
    [112] = Pad.B,
    ["z"] = Pad.SELECT,
    [122] = Pad.SELECT,
    ["x"] = Pad.START,
    [120] = Pad.START
}

local function keypressed(key)
    for k, v in pairs(keyButtons) do
        if k == key then
            keyEvents[#keyEvents + 1] = {"keydown", v}
        end
    end
end

local function keyreleased(key)
    for k, v in pairs(keyButtons) do
        if k == key then
            keyEvents[#keyEvents + 1] = {"keyup", v}
        end
    end
end

local function OnKey(timestamp, state, multi, keycode)
    --[[
    if yapre.platform == "emscripten" then
        keycode = emscripten_keycode_mapping:GetKeyCode(keycode)
    end
    --]]
    yapre.log.info(string.format("[OnKey] %i:%i:%i:%i", timestamp, state, multi, keycode))
    if state == 1 then
        keypressed(keycode)
        -- self:OnKey(timestamp, state, multi, keycode)
    elseif state == 2 then
        keyreleased(keycode)
    end
end

-- local app = require("main")
local imageData = yapre.Texture.new(width, height)

function Init()

    imageData = yapre.Texture.new(width * pixSize, height * pixSize)
    -- Nes:run()
    Nes:reset()
    yapre.SetRenderSize(width * pixSize, height * pixSize)
    yapre.BindKeyboardInputCallback("nes_on_key", OnKey)
    return true
end

local function drawScreen()
    --[[
    for x = 0, width-1 do
        for y = 0, height-1 do 
            imageData:SetPixel(x, y, 128, 0, 0, 255)
        end
    end
    --]]

    yapre.DrawSpriteWithImageData(imageData, {0, 0, 0}, {width, height}, 0, {1, 1, 1})
end

local function update()
    tickRatetmp = tickRatetmp + 1
    for i, v in ipairs(keyEvents) do
        Nes.pads[v[1]](Nes.pads, 1, v[2])
    end
    keyEvents = {}
    Nes:run_once()
    --[[
    local samples = Nes.cpu.apu.output
    for i = 1, #samples do
        sound:setSample(i, samples[i])
    end
    QS:queue(sound)
    QS:play()
    --]]
end

rate = rate * 1000
local d = 0

function Update(delta_ms)
    d = d + delta_ms
    if d < rate then
        return
    end

    d = 0
    update()

    local pxs = Nes.cpu.ppu.output_pixels
    local px = nil
    local iy = 0
    local ix = 0
    for y = 0, height - 1 do
        iy = y * width
        for x = 0, width - 1 do
            ix = iy + x
            px = pxs[ix + 1]
            imageData:SetPixel(x, y, math.floor(px[1]), math.floor(px[2]), math.floor(px[3]), 255)
        end
    end
    drawScreen()
end

function Deinit()
end
