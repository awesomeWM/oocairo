-- This example creates a small image by building up an array of pixels
-- into a string in the format Cairo uses internally, then creates a
-- Cairo surface from it.  In real use you'd probably be more likely to
-- use the 'image_surface_create_from_data' function to import data from
-- some other Lua library or C code.
--
-- The actual image created here isn't of much interest.  It just fades
-- the three primary colours in as the pixels get closer to three of the
-- corners, to test that the colour components are being used in the
-- expected order.

local Cairo = require "oocairo"

local SIZE = 256
local MAX = SIZE - 1

-- Each colour intensity is based on the distance from one corner of the
-- image.  In fact you could probably get the same effect by alpha blending
-- three radial gradients, with centers in the three corners.
local function colour (x, y)
    local distance = math.sqrt(x * x + y * y)
    distance = distance - distance % 1      -- round down to int
    distance = 255 - distance               -- closer means brighter
    if distance < 0 then distance = 0 end   -- clamp
    return distance
end

-- Create the image data, pixel by pixel.
local rows = {}     -- each item a binary string for a line of the image
local stride = Cairo.format_stride_for_width("rgb24", SIZE)
for y = 0, MAX do
    local pixels = {}   -- each item a four byte string for a pixel
    for x = 0, MAX do
        -- Red at top right, green at bottom right, blue at bottom left.
        local r = colour(MAX - x, y)
        local g = colour(MAX - x, MAX - y)
        local b = colour(x, MAX - y)
        if Cairo.BYTE_ORDER == "argb" then
            pixels[#pixels + 1] = string.char(0, r, g, b)
        else
            pixels[#pixels + 1] = string.char(b, g, r, 0)
        end
    end
    rows[#rows + 1] = table.concat(pixels)
end
local data = table.concat(rows)

-- Use the data to create a Cairo surface.  In this case we're just doing
-- it to use Cairo's PNG support to write it out, but we could use it as
-- a pattern or do drawing on top of it.
local surface = Cairo.image_surface_create_from_data(data, "rgb24", SIZE, SIZE,
                                                     stride)
surface:write_to_png("create-from-data.png")

-- vi:ts=4 sw=4 expandtab
