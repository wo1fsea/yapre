local yapre = yapre
local yecs = require("core.yecs")

-- dummy system
local dummy_system = {}
dummy_system.update_order = 0
function dummy_system:Init()
    yapre.log.info("dummy Init")
end

function dummy_system:Deinit()
    yapre.log.info("dummy Deinit")
end

function dummy_system:Update(delta_ms)
    yapre.log.info("dummy Update ", delta_ms)
end
yecs.System:Register("dummy", dummy_system)
