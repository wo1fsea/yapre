local yapre = yapre
local core = require("core")
local yecs = core.yecs

local app = {}

local splash_screen = require("core.splash_screen")
local game = require("app.game")
local game_inited = false

function app:Init()
    -- yapre.SetRenderSize(320, 240)
    yapre.SetKeepAspect(true)
    splash_screen:Show(function()
        game:Init()
        game_inited = true
    end)
    return true
end

function app:Update(delta_ms)
    yecs.World:UpdateAllWorlds(delta_ms)
    if not game_inited then
        return
    end

    game:Update(delta_ms)
end

function app:Deinit()
end

return app
