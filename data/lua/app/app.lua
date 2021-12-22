local yapre = yapre
local app = {}

local core = require("core")
local yecs = core.yecs

local splash_screen = require("core.splash_screen")
local game = require("app.game")

function app:Init()
    yapre.SetRenderSize(320, 240)
    splash_screen:Show(function()
        game:Init()
    end)
    return true
end

function app:Update(delta_ms)
    yecs.World:UpdateAllWorlds(delta_ms)
end

function app:Deinit()
end

return app
