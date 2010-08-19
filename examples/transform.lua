-- Draw a rectangle with different transformations to rotate and scale it.
-- It also has a translation applied, although that's always the same.
-- This also uses 'save' and 'restore' to push and pop the transformation
-- matrix, so that the translation doesn't have to be applied to a fresh
-- identity matrix each time round.

local Cairo = require "oocairo"

local PI = 2*math.asin(1)

local BOX_SZ, MARGIN = 300, 10
local IMG_CENTER = math.sqrt(2 * (BOX_SZ / 2) ^ 2) + MARGIN
local IMG_SZ = 2 * IMG_CENTER

local PARAMS = {
    { scale = 1, num_boxes = 7, color = { 0, 0, 0, 0.2 } },
    { scale = 0.55, num_boxes = 5, color = { 1, 0.3, 0.3, 0.3 } },
    { scale = 0.25, num_boxes = 3, color = { 0, 0, 0.8, 0.3 } },
}

local surface = Cairo.image_surface_create("rgb24", IMG_SZ, IMG_SZ)
local cr = Cairo.context_create(surface)

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

cr:translate(IMG_CENTER, IMG_CENTER)
for _, param in ipairs(PARAMS) do
    cr:save()
    cr:set_source_rgba(unpack(param.color))
    cr:scale(param.scale, param.scale)
    for _ = 1, param.num_boxes do
        cr:rotate(2 * PI / param.num_boxes)
        cr:rectangle(-BOX_SZ / 2, -BOX_SZ / 2, BOX_SZ, BOX_SZ)
        cr:fill()
    end
    cr:restore()
end

surface:write_to_png("transform.png")

-- vi:ts=4 sw=4 expandtab
