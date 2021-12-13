local palette_data = {}

local GetColorFromHexStr = function(str)
    local color = {
        a = 0,
        r = 0,
        g = 0,
        b = 0
    }
    local tmp = {}
    for cc in string.gmatch(str, "..") do
        table.insert(tmp, tonumber(cc, 16))
    end
    color.a = tmp[1] / 255
    color.r = tmp[2] / 255
    color.g = tmp[3] / 255
    color.b = tmp[4] / 255
    return color
end

-- https://lospec.com/palette-list/summers-past-16
local _colors = {'FF320011', 'FF5f3a60', 'FF876672', 'FFb7a39d', 'FFece8c2', 'FF6db7c3', 'FF5e80b2', 'FF627057',
                 'FF8da24e', 'FFd2cb3e', 'FFf7d554', 'FFe8bf92', 'FFe78c5b', 'FFc66f5e', 'FFc33846', 'FF933942'}

local colors = {}
for _, c in ipairs(_colors) do
    table.insert(colors, GetColorFromHexStr(c))
end

palette_data.colors = colors
palette_data.black = colors[1]
palette_data.white = colors[5]
palette_data.blue = colors[6]
palette_data.green = colors[10]
palette_data.red = colors[14]

palette_data.main_colors = {palette_data.black, palette_data.white, palette_data.blue, palette_data.green,
                            palette_data.red}

palette_data.foreground_color = colors[5]
palette_data.background_color = colors[16]
palette_data.border_color = colors[1]

return palette_data

