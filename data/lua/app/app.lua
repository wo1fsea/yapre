local app = {}

local core = require("core")
local yecs = core.yecs

local splash_screen = require("core.splash_screen")
local test_world = require("app.test_world")
local rewind_controller = require("app.rewind_controller")


function app:Init()
    yapre.SetRenderSize(320, 240)
    -- self:Deserialize() 
    local game_worlds = setmetatable({}, {__mode="v"})
    local make_test_world = function()
        local tw = test_world:Make()
        game_worlds["test_world"] = tw
    end

    --game_worlds["splash_screen"] = splash_screen:Show(make_test_world)
    splash_screen:Show(make_test_world)
    
    self.game_worlds = game_worlds
    self.rewind_controller = rewind_controller:Make(game_worlds)
    
    return true
end

function app:Update(delta_ms)
    yecs.World:UpdateAllWorlds(delta_ms)
end

function app:Deinit()
end

function app:Reload()
    -- local reload = require("utils.reload")
    -- reload.ReloadAll()
end


return app
