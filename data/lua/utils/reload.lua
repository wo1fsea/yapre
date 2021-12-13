local reload = {}

function reload.Reload(module_name)
    package.loaded[module_name] = nil
    require(module_name)
end

function reload.ReloadAll()
    local loaded_modules = {}
    for module_name, _ in pairs(package.loaded) do
        table.insert(loaded_modules, module_name)
    end
    for _, module_name in pairs(loaded_modules) do
        package.loaded[module_name] = nil
    end
    for _, module_name in pairs(loaded_modules) do
        require(module_name)
    end
end

return reload
