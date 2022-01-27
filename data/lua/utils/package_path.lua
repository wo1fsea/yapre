local function add_package_dir(package_dir)
    package.path = package.path .. string.format(";./lua/%s/?.lua;./lua/%s/?/_init.lua", package_dir, package_dir)
end

package.path = package.path .. ";./lua/?.lua;./lua/?/_init.lua"
add_package_dir("libs")
add_package_dir("utils")
