local palette_data = {}

local GetColorFromHexStr = function(str)
    local color = {a=0, r=0, g=0, b=0}
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
_colors = {
    'FF320011',
    'FF5f3a60',
    'FF876672',
    'FFb7a39d',
    'FFece8c2',
    'FF6db7c3',
    'FF5e80b2',
    'FF627057',
    'FF8da24e',
    'FFd2cb3e',
    'FFf7d554',
    'FFe8bf92',
    'FFe78c5b',
    'FFc66f5e',
    'FFc33846',
    'FF933942',
}

local colors = {}
for _, c in ipairs(_colors) do 
    table.insert(colors, GetColorFromHexStr(c))
end

palette_data.colors = colors

return palette_data

