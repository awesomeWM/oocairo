-- This tests the 'text_extents' and 'glyph_extents' methods, by drawing
-- shapes to describe the numbers returned.
--
-- The blue box shows the extent of the drawing caused by the text.  The
-- red line shows the advance from the current point before the text
-- was drawn to the place where the current point will be left afterwards.
-- The green line is the distance from the current point to the place where
-- the image of the text will be drawn.  A small blue circle shows the
-- current point before the text is drawn (the place you 'move_to').
--
-- The first line of text uses 'text_extents' and the second 'glyph_extents'.

local Cairo = require "oocairo"

local FONT_NAME, FONT_SIZE = "Sans", 90
local IMG_WD, IMG_HT = 390, 235

local function draw_extents (cr, x, y, ext)
    cr:save()

    -- Show width and height with light blue box round area they cover
    cr:set_source_rgb(0.7, 0.7, 1)
    cr:rectangle(x + ext.x_bearing, y + ext.y_bearing, ext.width, ext.height)
    cr:fill()

    -- Show x_advance and y_advance with a red line.
    cr:set_source_rgb(1, 0, 0)
    cr:move_to(x, y)
    cr:rel_line_to(ext.x_advance, ext.y_advance)
    cr:stroke()

    -- Show x_bearing and y_bearing with a green line
    cr:set_source_rgb(0, 0.7, 0)
    cr:move_to(x, y)
    cr:rel_line_to(ext.x_bearing, ext.y_bearing)
    cr:stroke()

    -- A small circle where the origin is.
    cr:set_source_rgb(0, 0, .8)
    cr:arc(x, y, 4, 0, math.asin(1) * 4)
    cr:fill()

    cr:restore()
end

local surface = Cairo.image_surface_create("rgb24", IMG_WD, IMG_HT)
local cr = Cairo.context_create(surface)

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

cr:set_source_rgb(0, 0, 0)
cr:select_font_face(FONT_NAME, "italic")
cr:set_font_size(FONT_SIZE)

local x, y = 20, 80
local text = "Xyzzy…"
draw_extents(cr, x, y, cr:text_extents(text))
cr:move_to(x, y)
cr:show_text(text)

-- The exact glyph numbers will depend on the font.  This is intended to
-- spell out 'fox', but the actual glyphs you get might be different.
x, y = 20, 180
local glyphs = { {73,x,y}, {82,x+100,y+20}, {91,x+200,y+40} }
draw_extents(cr, x, y, cr:glyph_extents(glyphs))
cr:show_glyphs(glyphs)

surface:write_to_png("text-extents.png")

-- vi:ts=4 sw=4 expandtab
