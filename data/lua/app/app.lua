
local core = require("core")
local yecs = core.yecs

local test_world = require("app.test_world")
local splash_world = require("core.splash_world")

local app = {}


function app:Init()
    yapre.SetRenderSize(320, 240)
    
    local new_test_world = function()
        self.test_world = test_world:New()
    end

    splash_world:New(new_test_world)

    return true
end

function app:Update(delta_ms)
    local worlds = {}
    
    for _, world in pairs(yecs.worlds) do
       table.insert(worlds, world)
    end

    for _, world in pairs(worlds) do
        world:Update(delta_ms)
    end
end

function app:Deinit()
end

function app:Reload()

end

return app
