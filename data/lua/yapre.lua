local yapre = yapre

require("lua.utils.package_path")

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

local app = require("app")
yapre.app = app

function Init()
    return app:Init()
end

function Update(delta_ms)
    app:Update(delta_ms)
end

function Deinit()
    app:Deinit()
end
