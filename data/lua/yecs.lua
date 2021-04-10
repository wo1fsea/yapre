local yecs = {}

yecs.worlds = {}

local World = {
    -- key = "",
    -- entities = {},
    -- systems = {},
    __metatable = "WorldMeta",
    __tostring = function(self) return string.format("<yecs-world: %s>", self.key) end,
}

function World:New(key)
    local world = yecs.worlds[key]
    if(world)
    then
        return world
    end

    world = setmetatable(
    {
        key = key,
        entities = {},
        systems = {},
    },
    self 
    )
    yecs.worlds[key] = world
    return world
end

function World:Destroy()
   yecs.worlds[self.key] = nil 
end

function World:Update(ms)
    for key, system in pairs(self.systems) do
        system.Update(ms)
    end
end

function World:AddEntity(entity)
    self.entities[entity.key] = entity
end

function World:RemoveEntity(entity)
    self.entities[entity.key] = nil
end

function World:AddSystem(system)  
    self.systems[system.key] = system
end

function World:RemoveSystem(system)
    self.systems[system.key] = nil 
end


-- entity

local entity_key = 0
local Entity = {
    key = "",
    __metatable = "EntityMeta",
    __tostring = function(self) return string.format("<yecs-world: %s>", self.name) end,

}

function Entity:New(components={})
    entity_key += 1
    local entity = setmetatable(
    {
        key = entity_key,
        components = components
        __index = components,
    },
    self 
    )

    return entity
end

function Entity:AddComponent(key, component)
    self.components[key] = components
end


local System = {}


return yecs
