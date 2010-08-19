-- This tests the 'text_path' and 'glyph_path' methods.  The output image
-- shows the outlines of some text, with the paths filled and then stroked
-- in a different colour.
--
-- The first line of text uses 'text_path' to get a path for a string, but
-- also uses a custom matrix transformation to get an unusual backwards
-- oblique effect.
--
-- The second line of text uses 'glyph_path', so each glyph is individually
-- positioned and selected by glyph ID instead of Unicode character.

local Cairo = require "oocairo"

local FONT_NAME, FONT_SIZE = "Sans", 200
local IMG_WD, IMG_HT = 350, 370

local function draw_outline_of_path (cr)
    -- Draw actual path, with curves in tact, in black.
    cr:set_source_rgb(0.8, 0.5, 0.8)
    cr:fill_preserve()
    cr:set_source_rgb(0, 0, 0)
    cr:set_line_width(4)
    cr:stroke()
end

local surface = Cairo.image_surface_create("rgb24", IMG_WD, IMG_HT)
local cr = Cairo.context_create(surface)

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

cr:select_font_face(FONT_NAME)
cr:set_font_size(FONT_SIZE)

-- Just for fun, adjust the matrix to do a shear transform on the font,
-- creating a kind of backwards oblique effect.
cr:save()
local mat = cr:get_font_matrix()
mat[3] = 60
cr:set_font_matrix(mat)

local x, y = 45, 165
cr:move_to(x, y)
cr:text_path("foo")
draw_outline_of_path(cr)
cr:restore()        -- restore normal font matrix, disabling shearing

-- The exact glyph numbers will depend on the font.  This is intended to
-- spell out 'bar', but the actual glyphs you get might be different.
x, y = 5, 335
cr:glyph_path({ {69,x,y}, {68,x+150,y+20}, {85,x+245,y-20} })
draw_outline_of_path(cr)

surface:write_to_png("text-path.png")

-- vi:ts=4 sw=4 expandtab
