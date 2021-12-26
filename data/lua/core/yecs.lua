local yecs = {}
local copy = require("utils.copy")
local uuid = require("utils.uuid")
local deep_copy = copy.deep_copy

-- world
yecs.worlds = {}

local World = {
    __metatable = 'WorldMeta',
    __tostring = function(self)
        return string.format('<yecs-world: %s>', self.key)
    end
}
World.__index = World

function World:New(key)
    local world = yecs.worlds[key]
    if world then
        return world
    end

    world = setmetatable({
        paused = false,
        update_delta = 0,
        key = key,
        entities = {},
        systems = {},
        system_update_queue = setmetatable({}, {
            __mode = 'v'
        })
    }, self)

    yecs.worlds[key] = world
    return world
end

function World:Destroy()
    yecs.worlds[self.key] = nil
    for _, system in pairs(self.systems) do
        system:Deinit()
    end
    self.entities = {}
    self.systems = {}
    self.system_update_queue = {}
end

function World:Update(delta_ms)
    self.update_delta = self.update_delta + delta_ms

    if self.paused then
        for _, system in ipairs(self.system_update_queue) do
            if system.update_when_paused then
                system:Update(0)
            end
        end
    else
        for _, system in ipairs(self.system_update_queue) do
            system:Update(self.update_delta)
        end
        self.update_delta = 0
    end
end

function World:UpdateAllWorlds(delta_ms)
    local worlds = {}

    for _, world in pairs(yecs.worlds) do
        table.insert(worlds, world)
    end

    for _, world in pairs(worlds) do
        world:Update(delta_ms)
    end
end

function World:Pause()
    self.paused = true
end

function World:Resume()
    self.paused = false
end

function World:AddEntity(entity)
    if getmetatable(entity) ~= "EntityMeta" then
        return
    end

    self.entities[entity.key] = entity
    entity.world = self
end

function World:AddEntities(entities)
    for _, entitie in pairs(entities) do
        self:AddEntity(entitie)
    end
end

function World:RemoveEntity(entity)
    self:RemoveEntityByKey(entity.key)
end

function World:RemoveEntityByKey(entity_key)
    local entity = self.entities[entity_key]

    if entity and entity.world == self then
        entity.world = nil
        self.entities[entity_key] = nil
    end
end

function World:AddSystem(system)
    if getmetatable(system) ~= "SystemMeta" then
        return
    end

    if system == nil or self.systems[system.key] ~= nil then
        return
    end

    system.world = self
    self.systems[system.key] = system
    table.insert(self.system_update_queue, system)
    table.sort(self.system_update_queue, function(s1, s2)
        return s1.update_order < s2.update_order
    end)

    system:Init()
end

function World:AddSystemByKey(system_key)
    local system = yecs.System:New(system_key)
    self:AddSystem(system)
end

function World:AddSystems(systems)
    for _, system in pairs(systems) do
        self:AddSystem(system)
    end
end

function World:AddSystemsByKeys(system_keys)
    for _, system_key in pairs(system_keys) do
        self:AddSystemByKey(system_key)
    end
end

function World:RemoveSystem(system)
    if getmetatable(system) ~= "SystemMeta" then
        return
    end
    self:RemoveSystemByKey(system.key)
end

function World:RemoveSystemByKey(system_key)
    local system = self.systems[system_key]
    if system == nil or system.world ~= self then
        return
    end

    system.world = nil
    self.systems[system_key] = nil
    system:Deinit()
    collectgarbage()

    system.system_update_queue = {}
    for _, system in pairs(self.systems) do
        table.insert(self.system_update_queue, system)
    end
    table.sort(self.system_update_queue, function(s1, s2)
        return s1.update_order < s2.update_order
    end)
end

function World:GetEntities(condition)
    local entities = {}

    for _, entity in pairs(self.entities) do
        if (not condition) or condition(entity) then
            table.insert(entities, entity)
        end
    end

    return entities
end

function World:GetEntity(condition)
    for _, entity in pairs(self.entities) do
        if (not condition) or condition(entity) then
            return entity
        end
    end

    return nil
end

function World:GetEntitiesWithComponent(component_key)
    return self:GetEntities(function(entity)
        return entity.components[component_key]
    end)
end

function World:GetEntitiesByTags(tags)
    return self:GetEntities(function(entity)
        if entity.tags == nil then
            return false
        end
        for _, tag in pairs(tags) do
            if entity.tags[tag] then
                return true
            end
        end
    end)
end

function World:GetEntityByTags(tags)
    return self:GetEntity(function(entity)
        if entity.tags == nil then
            return false
        end
        for _, tag in pairs(tags) do
            if entity.tags[tag] then
                return true
            end
        end
    end)
end

-- behavior
local behavior_templates = {}

local Behavior = {
    key = "",
    __metatable = "BehaviorMeta",
    __tostring = function(self)
        return string.format("<yecs-behavior: %s>", self.key)
    end
}

Behavior.__index = function(self, k)
    return self.behavior_funcs[k] or self.super_behavior[k]
end

function Behavior:Register(key, behavior_funcs)
    if type(behavior_funcs) ~= 'table' then
        return
    end
    behavior_templates[key] = behavior_funcs
end

function Behavior:New(key, super_behavior)
    local behavior_funcs = behavior_templates[key]
    assert(behavior_funcs, "cannot find behavior with key: " .. key)

    local behavior = setmetatable({
        key = key,
        behavior_funcs = behavior_funcs,
        super_behavior = super_behavior
    }, self)

    return behavior
end

-- entity
local Entity = {
    key = "",
    __metatable = "EntityMeta",
    __tostring = function(self)
        return string.format("<yecs-entity: %s>", self.key)
    end
}

Entity.__index = function(self, k)
    return Entity[k] or self._behavior[k] or self.components[k]
end
Entity.__newindex = function(self, k, v)
    if k == "world" then
        rawset(self, k, v)
    elseif self.components[k] == nil then
        yapre.log.info("can not add property to entity")
        -- rawset(self, k, v)
    elseif type(v) == "table" then
        local component = self.components[k]
        for ck, cv in pairs(v) do
            component[ck] = cv
        end
    end
end

function Entity:_BehaviorOnInit()
    local function _call_super(behavior)
        if behavior then
            _call_super(behavior.super_behavior)
            if behavior.OnInit then
                behavior.OnInit(self)
            end
        end
    end
    _call_super(self._behavior)
end

function Entity:New(component_keys, behavior_keys)
    component_keys = component_keys or {}
    behavior_keys = behavior_keys or {}

    local component_data = {}
    local behavior = {}
    for _, bk in ipairs(behavior_keys) do
        behavior = Behavior:New(bk, behavior)
    end

    local entity = setmetatable({
        key = uuid.new(),
        components = component_data,
        behavior_keys = copy.copy(behavior_keys),
        _behavior = behavior
    }, self)

    for _, component_key in pairs(component_keys) do
        local component = yecs.Component:New(component_key)

        if component ~= nil then
            component.entity = entity
            component_data[component.key] = component
        end
    end

    return entity
end

function Entity:AddComponent(component)
    local component = yecs.Component:New(component)

    if component and self.components[component.key] == nil then
        self.components[component.key] = component
    end
end

function Entity:AddComponents(components)
    for _, component in pairs(components) do
        self:AddComponent(component)
    end
end

function Entity:AddBehavior(behavior_key)
    self._behavior = Behavior:New(behavior_key, self._behavior)
    table.insert(self.behavior_keys, behavior_key)
end

function Entity:AddBehaviors(behavior_keys)
    for _, behavior_key in pairs(behavior_keys) do
        self:AddBehavior(behavior_key)
    end
end

function Entity:Serialize()
    local data = {}
    data.key = self.key

    local components = {}
    for c_k, c in pairs(self.components) do
        components[c_k] = c:Serialize()
    end

    data.components = components
    data.behavior_keys = copy.copy(self.behavior_keys)
    return data
end

function Entity:Deserialize(data)
    self.key = data.key
    for c_k, c_v in pairs(data.components) do
        self.components[c_k]:Deserialize(c_v)
    end
end

-- component
local component_templates = {}

local Component = {
    key = "",
    _operations = {},
    __metatable = "ComponentMeta",
    __tostring = function(self)
        return string.format("<yecs-component: %s>", self.key)
    end
}
Component.__index = function(self, k)
    return self._operations[k] or Component[k]
end
Component.__newindex = function(self, k, v)
    if type(v) ~= 'function' then
        rawset(self, k, v)
    else
        self._operations[k] = v
    end
end

function Component:Register(key, data)
    if component_templates[key] ~= nil then
        return
    end
    if type(data) ~= 'table' then
        return
    end

    data["key"] = key
    if data._operations == nil then
        data._operations = {}
    end

    component_templates[key] = data
end

function Component:New(key)
    local component_data = component_templates[key]
    assert(component_data, "cannot find component with key: " .. key)

    local component = setmetatable(deep_copy(component_data), self)
    return component
end

function Component:Serialize()
    local data = {}

    for k, v in pairs(self) do
        if type(k) == "string" and string.sub(k, 1, 1) ~= "_" and k ~= "entity" then
            data[k] = v
        end
    end

    return deep_copy(data)
end

function Component:Deserialize(data)
    for k, v in pairs(data) do
        self[k] = v
    end
end

-- system
local system_templates = {}
local System = {
    key = "",
    update_order = 1024,
    update_when_paused = false,
    __metatable = "SystemMeta",
    __tostring = function(self)
        return string.format("<yecs-system: %s>", self.key)
    end
}
System.__index = System

function System:Register(key, data)
    if type(data) ~= 'table' then
        return
    end
    data["key"] = key
    system_templates[key] = data
end

function System:New(key)
    local system_data = system_templates[key]
    if system_data == nil then
        return nil
    end

    local system = setmetatable(deep_copy(system_data), self)
    return system
end

function System:Update(delta_ms)
    yapre.log.info(self, "Update", delta_ms)
end

function System:Init()
end

function System:Deinit()
end

-- EntityFactory
local EntityFactory = {
    entity_models = {}
}

function EntityFactory:Make(entity_type, ex_behavior_keys)
    local data = self.entity_models[entity_type]
    if not data then
        return nil
    end

    local component_keys = data.component_keys
    local behavior_keys = data.behavior_keys

    local entity = yecs.Entity:New(component_keys, behavior_keys)
    if not entity then
        return nil
    end

    if ex_behavior_keys then
        for _, behavior_key in pairs(ex_behavior_keys) do
            entity:AddBehavior(behavior_key)
        end
    end

    if entity.Init then
        entity:Init()
    end

    -- behaviors OnInit
    entity:_BehaviorOnInit()

    return entity
end

function EntityFactory:Register(entity_type, component_keys, behavior_keys)
    self.entity_models[entity_type] = {
        component_keys = component_keys,
        behavior_keys = behavior_keys
    }
end

yecs.World = World
yecs.Entity = Entity
yecs.Component = Component
yecs.System = System
yecs.EntityFactory = EntityFactory
yecs.Behavior = Behavior

return yecs
