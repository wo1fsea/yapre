local yecs 

local 

local __worldMeta = 
{
    __index = {
        add = tiny.add,
        addEntity = tiny.addEntity,
        addSystem = tiny.addSystem,
        remove = tiny.remove,
        removeEntity = tiny.removeEntity,
        removeSystem = tiny.removeSystem,
        refresh = tiny.refresh,
        update = tiny.update,
        clearEntities = tiny.clearEntities,
        clearSystems = tiny.clearSystems,
        getEntityCount = tiny.getEntityCount,
        getSystemCount = tiny.getSystemCount,
        setSystemIndex = tiny.setSystemIndex
    },
    __tostring = function()
        return "<yecs-world>"
    end
}

yecs.NewWorld = function(name)
    local world = setmetatable(
    {
        entities = {},
        systems = {},
    }, 
    __worldMeta
    )

    return world
end



return yecs
