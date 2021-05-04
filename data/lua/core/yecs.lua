local yecs = {}
local copy = require("utils.copy")
local uuid = require("utils.uuid")
local deep_copy = copy.deep_copy

-- world
yecs.worlds = {}

local World = {
    __metatable = 'WorldMeta',
    __tostring = function(self) return string.format('<yecs-world: %s>', self.key) end,
}
World.__index = World

function World:New(key)
    local world = yecs.worlds[key]
    if world then return world end

    world = setmetatable(
    {
        paused=false,
        update_delta=0,
        key=key,
        entities={},
        systems={},
        system_update_queue=setmetatable({}, {__mode='v'}),
    },
    self 
    )
    
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

function World:Pause()
    self.paused = true 
end

function World:Resume()
    self.paused = false
end

function World:AddEntity(entity)
    self.entities[entity.key] = entity
    entity.world = self
end

function World:RemoveEntity(entity)
    local entity_key = nil
    if getmetatable(entity) == "EntityMeta" then
        entity_key = entity.key
    else
        entity_key = entity
        entity = self.entities[entity_key]
    end
    
    if entity and entity.world == self then
        entity.world = nil
        self.entities[entity_key] = nil
    end
end

function World:AddSystem(system)  
    if getmetatable(system) ~= "SystemMeta" then
        system = yecs.System:New(system)
    end

    if system == nil or self.systems[system.key] ~= nil then return end
    
    system.world = self
    self.systems[system.key] = system
    table.insert(self.system_update_queue, system)
    table.sort(self.system_update_queue, function(s1, s2) return s1.update_order < s2.update_order end)

    system:Init()
end

function World:AddSystems(systems)
    for _, system in pairs(systems) do
        self:AddSystem(system)
    end
end

function World:RemoveSystem(system)
    if getmetatable(system) ~= "SystemMeta" then
        system = self.systems[system]
    end 

    if system == nil or system.world ~= self then
        return 
    end

    system.world = nil
    self.systems[system.key] = nil 
    system:Deinit()
    collectgarbage()
    
    system.system_update_queue = {}
    for _, system in pairs(self.systems) do
        table.insert(self.system_update_queue, system)
    end
    table.sort(self.system_update_queue, function(s1, s2) return s1.update_order < s2.update_order end)
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

function World:GetEntitiesByTags(tags)
    return self:GetEntities(
    function(entity)
        if entity.tags == nil then return false end
        for _, tag in pairs(tags) do
            if entity.tags[tag] then return true end
        end
    end
    )
end

function World:GetEntityByTags(tags)
    return self:GetEntity(
    function(entity)
        if entity.tags == nil then return false end
        for _, tag in pairs(tags) do
            if entity.tags[tag] then return true end
        end
    end
    )
end


-- behavior
local Behavior = {
    behaviors={},
}

function Behavior:Register(key, behavior)
	if type(behavior) ~= 'table' then return end
    self.behaviors[key] = behavior
end

function Behavior:Get(key)
    if not key then return {} end
    return self.behaviors[key] or {}
end


-- entity
local Entity = {
    key = "",
    __metatable = "EntityMeta",
    __tostring = function(self) return string.format("<yecs-entity: %s>", self.key) end,
}
Entity.__index = function(self, k) return Entity[k] or self._behavior[k] or self.components[k] end
Entity.__newindex = function(self, k, v)
    if k == "world" then
        rawset(self, k, v)
    elseif self.components[k] == nil then
        print("can not add property to entity")
        -- rawset(self, k, v)
    elseif type(v) == "table" then
        local component = self.components[k]
        for ck, cv in pairs(v) do
            component[ck] = cv
        end
    end
end

function Entity:New(components, behavior_keys)
    components = components or {}
    local component_data = {}
    local _behavior = {} 
    for _, bk in ipairs(behavior_keys) do
        for k, v in pairs(Behavior:Get(bk)) do
            _behavior[k] = v
        end
    end

    local entity = setmetatable(
    {
        key = uuid.new(),
        components = component_data,
        behavior_keys = copy.copy(behavior_keys),
        _behavior = _behavior,
    },
    self 
    )

    for k, v in pairs(components) do
        if getmetatable(v) ~= "ComponentMeta" then
            v = yecs.Component:New(v) 
        end

        if v ~= nil then 
            v.entity = entity
            component_data[v.key] = v 
        end
    end

    return entity
end

function Entity:AddComponent(component)
    if getmetatable(component) ~= "ComponentMeta" then
        component = yecs.Component:New(component)
    end
     
    if component and self.components[component.key] == nil then
        self.components[component.key] = component
    end
end

function Entity:AddBehavior(behavior_key)
    for k, v in pairs(Behavior:Get(behavior_key)) do
        self._behavior[k] = v
    end
    table.insert(self.behavior_keys, behavior_key)
end

function Entity:Serialize()
    local data = {}
    data.key = self.key

    local components = {}
    for c_k, c in pairs(self.components) do
        components[c_k] = c:Serialize()
    end

    data.components = components
    data.behavior_keys = self.behavior_keys
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
    __tostring = function(self) return string.format("<yecs-component: %s>", self.key) end,
}
Component.__index = function(self, k) return self._operations[k] or Component[k] end
Component.__newindex = function(self, k, v) 
    if type(v) ~= 'function' then
        rawset(self, k, v)
    else
        self._operations[k] = v
    end
end

function Component:Register(key, data)
	if component_templates[key] ~= nil then return end
	if type(data) ~= 'table' then return end
    
    data["key"] = key
    if data._operations == nil then
        data._operations = {}
    end

    component_templates[key] = data
end

function Component:New(key)
    local component_data = component_templates[key]
    if component_data == nil then return nil end
    
    local component = setmetatable
    (
       deep_copy(component_data),
       self
    )
    return component
end

function Component:Serialize()
    local data = {}

    for k, v in pairs(self) do
        if type(k) == "string" and string.sub(k, 1, 1) ~= "_" and k ~= "entity"  then
            data[k] = v
        end
    end

    return data
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
    __tostring = function(self) return string.format("<yecs-system: %s>", self.key) end,
}
System.__index = System

function System:Register(key, data)
	if type(data) ~= 'table' then return end
    data["key"] = key
    system_templates[key] = data
end

function System:New(key)
    local system_data = system_templates[key]
    if system_data == nil then return nil end

    local system = setmetatable
    (
       deep_copy(system_data),
       self
    )
    return system
end

function System:Update(delta_ms)
    print(self, "Update", delta_ms)
end

function System:Init()
end

function System:Deinit()
end


-- EntityFactory
local EntityFactory = {
    entity_models={},
}

function EntityFactory:Make(entity_type, ex_behavior_keys)
    local data = self.entity_models[entity_type]
    if not data then return nil end

    local components = data.components
    local behavior_keys = data.behavior_keys

    local entity = yecs.Entity:New(components, behavior_keys)
    if not entity then return nil end
    
    if ex_behavior_keys then
        for _, behavior_key in pairs(ex_behavior_keys) do
            entity:AddBehavior(behavior_key)
        end
    end
    
    if entity.Init then
        entity:Init()
    end

    if entity.OnInit then
        entity:OnInit()
    end

    return entity
end

function EntityFactory:Register(entity_type, components, behavior_keys)
    self.entity_models[entity_type] = {components=components, behavior_keys=behavior_keys}
end

yecs.World = World
yecs.Entity = Entity
yecs.Component = Component 
yecs.System = System
yecs.EntityFactory = EntityFactory
yecs.Behavior = Behavior


return yecs
