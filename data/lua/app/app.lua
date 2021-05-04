
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
    -- self:Deserialize() 
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
