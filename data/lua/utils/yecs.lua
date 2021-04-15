local yecs = {}

function deep_copy(obj, seen)
	-- Handle non-tables and previously-seen tables.
	if type(obj) ~= 'table' then return obj end
	if seen and seen[obj] then return seen[obj] end

	-- New table; mark it as seen an copy recursively.
	local s = seen or {}
	local res = {}
	s[obj] = res
	for k, v in next, obj do res[deep_copy(k, s)] = deep_copy(v, s) end
	return setmetatable(res, getmetatable(obj))
end

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
        key = key,
        entities = {},
        systems = {},
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
    for _, system in ipairs(self.system_update_queue) do
        system:Update(delta_ms)
    end
end

function World:AddEntity(entity)
    self.entities[entity.key] = entity
end

function World:RemoveEntity(entity)
    if getmetatable(entity) == "EntityMeta" then
        entity = entity.key
    end

    self.entities[entity] = nil
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

-- entity

local entity_key = 0
local Entity = {
    key = "",
    __metatable = "EntityMeta",
    __tostring = function(self) return string.format("<yecs-entity: %s>", self.key) end,
}
Entity.__index = function(self, k) return Entity[k] or self.components[k] end

function Entity:New(components)
    components = components or {}
    local component_data = {}
   
    component_keys = {}
    for k, v in pairs(components) do
        if getmetatable(v) ~= "ComponentMeta" then
            component_data[v] = yecs.Component:New(v) 
        else
            component_data[v.key] = v 
        end
    end

    entity_key = entity_key + 1
    local entity = setmetatable(
    {
        key = tostring(entity_key),
        components = component_data,
    },
    self 
    )

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

-- component
local component_templates = {}

local Component = {
    key = "",
    __metatable = "ComponentMeta",
    __tostring = function(self) return string.format("<yecs-component: %s>", self.key) end,
}
Component.__index = Component

function Component:Register(key, data)
	if component_templates[key] ~= nil then return end
	if type(data) ~= 'table' then return end

    data["key"] = key
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

-- system
local system_templates = {}
local System = {
    key = "",
    update_order = 1024,
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
    system_data = system_templates[key]
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

yecs.World = World
yecs.Entity = Entity
yecs.Component = Component 
yecs.System = System
return yecs
