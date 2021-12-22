local serialization = {}
local yecs = require("core.yecs")
local copy = require("utils.copy")
local debug_log = require("utils.debug_log")

function serialization:DumpWorld(world)
    local world_data = {}

    world_data.paused = world.paused
    world_data.update_delta = world.update_delta
    world_data.key = world.key

    local systems = {}
    for system_key, _ in pairs(world.systems) do
        table.insert(systems, system_key)
    end
    world_data.systems = systems

    local entities_data = {}
    for entity_key, entity in pairs(world.entities) do
        entities_data[entity_key] = entity:Serialize()
    end

    world_data.entities = entities_data

    return world_data
end

function serialization:LoadWorld(world_data)
    local world = yecs.World:New(world_data.key)
    world:AddSystemsByKeys(world_data.systems)

    world.paused = world_data.paused
    world.update_delta = world_data.update_delta

    local entities_data = world_data.entities
    local entities = self:MakeEntities(entities_data)

    for _, entity in pairs(entities) do
        world:AddEntity(entity)
    end

    for e_key, entity in pairs(entities) do
        entity:Deserialize(entities_data[e_key])
    end

    return world
end

function serialization:MakeEntities(entities_data)
    local entities = {}

    for entity_key, entity_data in pairs(entities_data) do
        local components = {}
        for c_key, _ in pairs(entity_data.components) do
            table.insert(components, c_key)
        end

        local entity = yecs.Entity:New(components, entity_data.behavior_keys)
        if entity.Init then
            entity:Init()
        end

        -- behaviors OnInit
        entity:_BehaviorOnInit()

        entity.key = entity_key
        entities[entity_key] = entity
    end

    return entities
end

function serialization:DumpData(obj, seen)
    if type(obj) ~= "table" then
        if type(obj) == "function" then
            debug_log.log("unable to dump function")
            return nil
        end
        return obj
    end

    if seen and seen[obj] then
        return seen[obj]
    end

    local s = seen or {}
    local data = {}
    s[obj] = data

    for k, v in pairs(obj) do
        if type(k) == "string" and string.sub(k, 1, 1) ~= "_" or type(k) == "number" then
            data[k] = self:DumpData(v, s)
        end
    end

    return data
end

return serialization
