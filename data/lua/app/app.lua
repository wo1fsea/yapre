
local core = require("core")
local yecs = core.yecs
local reload = require("utils.reload")

local test_world = require("app.test_world")
local splash_screen = require("core.splash_screen")

local rewind_controller = require("app.rewind_controller")

local app = {}

local s = require("core.serialization")

function app:Init()
    yapre.SetRenderSize(320, 240)
    -- self:Deserialize() 
    local make_test_world = function()
        self.test_world = test_world:Make()
        self.rewind_controller = rewind_controller:Make(self.test_world)
    end

    splash_screen:Show(make_test_world)
    return true
end

function app:Update(delta_ms)
    yecs.World:UpdateAllWorlds(delta_ms)
end

function app:Deinit()
end

function app:Reload()
    reload.ReloadAll()
end

function app:Test()
end

function app:Deserialize()
    if app.test_world then
        app.test_world:Destroy()
        app.test_world = nil
    end

    local data = table.load("./dump.lua")
    app.test_world = s:LoadWorld(data)
end

function app:Serialize()
    local data = s:DumpWorld(app.test_world)
    table.save(data, "./dump.lua")
end


return app
