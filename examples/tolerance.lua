-- Draw curves using different 'tolerance' values, just to see what happens.
-- The tolerance value decreases down the image.

local Cairo = require "oocairo"

local CURVE_HT, MARGIN = 165, 10
local IMG_WD, IMG_HT = 270, CURVE_HT * 3 + MARGIN * 4

local surface = Cairo.image_surface_create("rgb24", IMG_WD, IMG_HT)
local cr = Cairo.context_create(surface)

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

local x, y = MARGIN, MARGIN
for _, tolerance in ipairs{ 0.1, 2.5, 10 } do
    cr:set_tolerance(tolerance)
    cr:move_to(x, y)
    cr:curve_to(x + 2.5*CURVE_HT, y + 0.1*CURVE_HT,
                x + 1.5*CURVE_HT, y + 0.7*CURVE_HT,
                x, y + CURVE_HT)
    cr:set_source_rgb(0, 0, 0)
    cr:stroke()
    y = y + CURVE_HT + MARGIN
end

surface:write_to_png("tolerance.png")

-- vi:ts=4 sw=4 expandtab
