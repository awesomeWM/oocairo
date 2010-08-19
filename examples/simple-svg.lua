-- This example is just here so that you can copy and paste the few lines
-- of code you need to output SVG.  It doesn't draw anything interesting.

local Cairo = require "oocairo"

-- This check isn't normally necessary, but it can be used if you want to
-- write a program that works with or without SVG support.  I think most
-- Cairo installations will have the SVG surface feature enabled.
if not Cairo.HAS_SVG_SURFACE then
    error("This example requires a Cairo library with SVG support enabled")
end

-- The width and height given here are measured in points.
local surface = Cairo.svg_surface_create("simple-svg.svg", 300, 200)
local cr = Cairo.context_create(surface)

cr:set_source_rgb(1, 0, 1)   -- purple

cr:move_to(100, 10)          -- triangle
cr:line_to(300, 150)
cr:line_to(50, 100)
cr:fill()

-- This forces the output to be written right away, although it'll
-- happen automatically when the surface gets GCed anyway.  You only
-- need to call it explicitly if you want to ensure the output file
-- appears as soon as you've finished drawing on it.
surface:finish()

-- vi:ts=4 sw=4 expandtab
