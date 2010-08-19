-- A simple example which just draws some arbitrary shapes.

local Cairo = require "oocairo"

local surface = Cairo.image_surface_create("rgb24", 200, 200)
local cr = Cairo.context_create(surface)

cr:set_source_rgb(1, 1, 1)
cr:paint()

cr:move_to(0, 0)
cr:line_to(190, 100)
cr:line_to(100, 185)
cr:line_to(200, 200)
cr:line_to(30, 130)
cr:close_path()
cr:set_source_rgb(0.8, 0.4, 1)
cr:fill()

cr:move_to(180, 30)
cr:line_to(100, 20)
cr:line_to(80, 120)
cr:set_source_rgb(0.5, 0.7, 0.3)
cr:fill_preserve()
cr:set_source_rgb(0, 0, 0)
cr:set_line_width(3)
cr:stroke()

surface:write_to_png("simple-example.png")

-- vi:ts=4 sw=4 expandtab
