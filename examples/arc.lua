-- Show some of the possibilities of the 'arc' and 'arc_negative' methods.
--
-- The output image has three rows of shapes, the ones in each column being
-- drawn with the same pair of starting and ending angles, and the ones in
-- each row drawn with the same commands using those angles.  The first row
-- uses the 'arc' method, the second row is the same but with additional line
-- segments making the path start and end at the center of the circle, to
-- give a 'pie wedge' effect, and the last row is the same as the first but
-- uses the 'arc_negative' method.

local Cairo = require "oocairo"

local PI = 2*math.asin(1)

-- These are the ranges of angles drawn in each arc, in radians.
local ANGLES = { {0,2*PI}, {0,PI}, {PI,0}, {PI/3,1.5*PI}, {1.5*PI,PI/3} }

local RADIUS, MARGIN = 40, 15
local IMG_WD = #ANGLES*2*RADIUS + (#ANGLES+1)*MARGIN
local IMG_HT = 6*RADIUS + 4*MARGIN

local surface = Cairo.image_surface_create("rgb24", IMG_WD, IMG_HT)
local cr = Cairo.context_create(surface)

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

for i, ang in ipairs(ANGLES) do
    -- Normal arc with no start or end point.
    local x = i*MARGIN + i*2*RADIUS - RADIUS
    local y = MARGIN + RADIUS
    cr:new_sub_path()
    cr:arc(x, y, RADIUS, ang[1], ang[2])

    -- Arc with start and end points connected to center (pie-slice effect).
    y = y + MARGIN + 2*RADIUS
    cr:new_sub_path()
    cr:move_to(x, y)
    cr:arc(x, y, RADIUS, ang[1], ang[2])
    cr:close_path()

    -- Negative arc.
    y = y + MARGIN + 2*RADIUS
    cr:new_sub_path()
    cr:arc_negative(x, y, RADIUS, ang[1], ang[2])
end

-- Draw the whole lot at once.
cr:set_source_rgb(.5, .5, .5)
cr:fill_preserve()
cr:set_source_rgb(.3, 0, 0)
cr:stroke()

-- As a test of different ways to write the output, this particular example
-- program writes the finished PNG file through a Lua filehandle.
local fh = assert(io.open("arc.png", "wb"))
surface:write_to_png(fh)
fh:close()

-- vi:ts=4 sw=4 expandtab
