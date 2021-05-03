local copy_module = {}

local function deep_copy(obj, seen)
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

local function copy(obj)
	if type(obj) ~= 'table' then return obj end
    local c = {}
	for k, v in next, obj do res[k] = v end
	return setmetatable(c, getmetatable(obj))
end

copy_module.deep_copy = deep_copy 
copy_module.copy = copy

return copy_module
