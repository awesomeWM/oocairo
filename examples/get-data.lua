-- This example demonstrates using the 'get_data' method on an image
-- surface to retrieve the binary data for an image.  Typically you
-- would want to do that in order to pass the data to some other library,
-- but in this case we'll write it out in a file format that Cairo doesn't
-- know about, namely PPM, which most image viewers should understand.

local Cairo = require "oocairo"

local IMAGE_WD, IMAGE_HT = 432, 234

-- Write out image data as a PPM file.  PPM is the true-colour variant of
-- the PNM class of formats.  We're using the binary version of PPM here,
-- which has three bytes per pixel, rather than three ASCII numbers per
-- pixel.  This function assumes the data is for an RGB24 or ARGB32 image.
local function write_ppm (filename, width, data, stride)
    local fh = assert(io.open(filename, "wb"))
    local height = data:len() / stride
    fh:write("P6\n", width, " ", height, "\n", "255\n")     -- header

    for y = 0, height - 1 do
        for x = 0, width - 1 do
            local offset = y * stride + x * 4
            -- Cairo stores pixel components using the native machine's
            -- byte order conventions, so to work on all platforms we need
            -- to reverse the bytes on little-endian platforms.  This is why
            -- the 'get_data' method returns an endianness indicator.
            local a, r, g, b
            if Cairo.BYTE_ORDER == "argb" then  -- big endian
                a, r, g, b = data:byte(offset + 1, offset + 4)
            else                                -- little endian
                b, g, r, a = data:byte(offset + 1, offset + 4)
            end
            fh:write(string.char(r, g, b))
        end
    end

    fh:close()
end

local surface = Cairo.image_surface_create("rgb24", IMAGE_WD, IMAGE_HT)
local cr = Cairo.context_create(surface)

-- Draw a gradient designed to include lots of different colours (mainly
-- to confirm that the three colour components come out the right way
-- round in the output file).
local grad = Cairo.pattern_create_linear(0, 0, IMAGE_WD, IMAGE_HT)
grad:add_color_stop_rgb(0,  1, 0, 0)
grad:add_color_stop_rgb(.5, 0, 1, 0)
grad:add_color_stop_rgb(1,  0, 0, 1)
cr:set_source(grad)
cr:paint()

write_ppm("get-data.ppm", surface:get_width(), surface:get_data())

-- vi:ts=4 sw=4 expandtab
