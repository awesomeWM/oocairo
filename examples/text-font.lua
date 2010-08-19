-- Show some different font family and style selections.

local Cairo = require "oocairo"

local IMG_WD, IMG_HT = 470, 540

local surface = Cairo.image_surface_create("rgb24", IMG_WD, IMG_HT)
local cr = Cairo.context_create(surface)

local function draw_dot (x, y)
    cr:save()
    cr:move_to(x, y)
    cr:rel_line_to(0, 0)
    cr:set_line_width(5)
    cr:set_source_rgb(1, 0, 0)
    cr:set_line_cap("round")
    cr:stroke()
    cr:restore()
end

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

local START_X = 20
local x, y = START_X, 40
cr:set_font_size(30)
for _, family in ipairs{ "Sans", "Times New Roman" } do
    for _, slant in ipairs{ "normal", "italic", "oblique" } do
        for _, weight in ipairs{ "normal", "bold" } do
            cr:select_font_face(family, slant, weight)
            local text = family .. ", " .. slant .. ", " .. weight
            draw_dot(x, y)
            cr:move_to(x, y)
            cr:set_source_rgb(0, 0, 0)
            cr:show_text(text)
            local extents = cr:font_extents()
            x = START_X
            y = y + extents.height * 1.2
        end
    end
    y = y + 10
end

surface:write_to_png("text-font.png")

-- vi:ts=4 sw=4 expandtab
