-- This demonstrates the different styles of line endings and joins,
-- and some dash patterns.

local Cairo = require "oocairo"

local DEMO_WD, DEMO_HT, LINE_WD, MARGIN = 120, 50, 35, 50
local IMG_WD, IMG_HT = 515, 250

local surface = Cairo.image_surface_create("rgb24", IMG_WD, IMG_HT)
local cr = Cairo.context_create(surface)

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

-- Line join style.
local x, y = MARGIN/2, IMG_HT - MARGIN/2
for _, joinstyle in ipairs{ "miter", "round", "bevel" } do
    cr:move_to(x, y)
    cr:line_to(x + DEMO_WD / 2, y - DEMO_HT)
    cr:line_to(x + DEMO_WD, y)
    cr:set_source_rgb(0, 0, 0)
    cr:set_line_join(joinstyle)
    cr:set_line_width(LINE_WD)
    cr:stroke_preserve()
    cr:set_source_rgb(1, 0.5, 0.5)
    cr:set_line_join("miter")
    cr:set_line_width(2)
    cr:stroke()
    x = x + DEMO_WD + MARGIN
end

-- Line cap style.
x = MARGIN/2
y = y - DEMO_HT - MARGIN - LINE_WD/2
for _, capstyle in ipairs{ "butt", "round", "square" } do
    cr:move_to(x, y)
    cr:line_to(x + DEMO_WD, y)
    cr:set_source_rgb(0, 0, 0)
    cr:set_line_cap(capstyle)
    cr:set_line_width(LINE_WD)
    cr:stroke_preserve()
    cr:set_source_rgb(1, 0.5, 0.5)
    cr:set_line_cap("butt")
    cr:set_line_width(2)
    cr:stroke()
    x = x + DEMO_WD + MARGIN
end

-- Dash patterns.
x = MARGIN/2
y = y - MARGIN/2 - LINE_WD/2
cr:set_line_width(2)
cr:set_source_rgb(0, 0, 0);
for _, dashpat in ipairs{ {}, {3}, {3,6,9} } do
    for offset = 0, #dashpat do
        cr:set_dash(dashpat, offset)
        cr:move_to(x, y)
        cr:line_to(IMG_WD - MARGIN, y)
        cr:stroke()
        y = y - 5
    end
    y = y - 5
end

surface:write_to_png("stroke-style.png")

-- vi:ts=4 sw=4 expandtab
