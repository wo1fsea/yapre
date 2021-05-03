
local core = require("core")
local yecs = core.yecs
local reload = require("utils.reload")

local test_world = require("app.test_world")
local splash_world = require("core.splash_world")

local app = {}

local s = require("core.serialization")
require("utils.table_save")

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
    reload.ReloadAll()
end

function app:Test()

    local data = s:DumpWorld(app.test_world)
    local r = table.save(data, "./dump.lua")
    app.test_world:Destroy()

    local d2 = table.load("./dump.lua")
    app.w2 = s:LoadWorld(d2)
    for k, v in pairs(yecs.worlds) do 
        print("f", k)
    end
end

return app
