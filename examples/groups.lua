-- Example of how to use 'push_group' and 'pop_group_to_source' to composite
-- a bunch of drawing commands onto a background with a single level of
-- alpha blending, and also demonstrate the difference between the
-- 'get_target' and 'get_group_target' methods.

local Cairo = require "oocairo"

local PI = 2*math.asin(1)

local IMG_WD, IMG_HT = 450, 450

local surface = Cairo.image_surface_create("rgb24", IMG_WD, IMG_HT)
local cr = Cairo.context_create(surface)

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

-- Black cross to show through the translucent circles.
cr:set_source_rgb(0, 0, 0)
cr:move_to(0, 0)
cr:line_to(IMG_WD, IMG_HT)
cr:move_to(0, IMG_HT)
cr:line_to(IMG_WD, 0)
cr:set_line_width(20)
cr:stroke()

cr:push_group()
for _, v in ipairs{ {200,1,0,0}, {150,0,1,0}, {100,0,0,1}, {50,0,0,0} } do
    local radius, r, g, b = unpack(v)
    cr:arc(IMG_WD/2, IMG_HT/2, radius, 0, 2*PI)
    cr:set_source_rgb(r, g, b)
    cr:fill()
end

local function draw_text_on_surface (surface, y, text)
    local tmpcr = Cairo.context_create(surface)
    tmpcr:move_to(20, y)
    tmpcr:set_source_rgb(0, 0, 0)
    tmpcr:set_font_size(40)
    tmpcr:show_text(text)
end
draw_text_on_surface(cr:get_target(), IMG_HT / 2 - 70, "get_target")
draw_text_on_surface(cr:get_group_target(), IMG_HT / 2 + 80, "get_group_target")

cr:pop_group_to_source()
cr:paint_with_alpha(0.5)

surface:write_to_png("groups.png")

-- vi:ts=4 sw=4 expandtab
