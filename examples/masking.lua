-- Demonstrate some nice effects which can be achieved with the mask,
-- using it to composite photographs onto a background.  Two of them have
-- a linear and radial gradient as their masks, making the pictures fade
-- out, and the third uses a mask shape created by filling a path.
--
-- This also demonstrates how to load JPEG images, which Cairo doesn't
-- support directly.  It's a bit of a hack, but it works: the JPEGs are
-- loaded with GD, and then saved into a PNG file in a string (to avoid
-- the need for a temporary file) and then loaded back into Cairo from
-- the string.

local Cairo = require "oocairo"

-- This module is required for loading images from data stored in a string.

local IMG_WD, IMG_HT = 400, 300
local PI = 2 * math.asin(1)

-- This requires the 'memoryfile' Lua module, the GD library, and the Lua
-- module for GD.  We need them for loading the JPEG images.  These modules
-- are both available from LuaForge:
--     http://luaforge.net/projects/memoryfile/
--     http://luaforge.net/projects/lua-gd/
local MemFile
do
    local ok = pcall(require, "gd")
    if not ok then
        print("The Lua 'gd' module is required for this example.")
        os.exit(1)
    end
    ok, MemFile = pcall(require, "memoryfile")
    if not ok then
        print("The Lua 'memoryfile' module is required for this example.")
        os.exit(1)
    end
end

local function load_jpeg (filename)
    local img = assert(gd.createFromJpeg(filename),
                       "error loading JPEG: " .. filename)
    local fh = MemFile.open(img:pngStr())
    local surface = Cairo.image_surface_create_from_png(fh)
    fh:close()      -- not strictly necessary, but frees memory earlier
    return surface
end

local function make_snow_gradient ()
    -- Fade from full opacity at the bottom of to full transparency at the top.
    local gradient = Cairo.pattern_create_linear(0, 0, 0, IMG_HT)
    gradient:add_color_stop_rgba(0, 0, 0, 0, 0)
    gradient:add_color_stop_rgba(0.3, 0, 0, 0, 0)
    gradient:add_color_stop_rgba(0.8, 0, 0, 0, 1)
    return gradient
end

local function make_flower_gradient (size)
    -- Fade from full opacity in top left corner to more transparent in all
    -- directions.
    local gradient = Cairo.pattern_create_radial(size, size, 0,
                                                 size, size, size)
    gradient:add_color_stop_rgba(0.0, 0, 0, 0, 1)
    gradient:add_color_stop_rgba(0.8, 0, 0, 0, 1)
    gradient:add_color_stop_rgba(1.0, 0, 0, 0, 0)
    return gradient
end

local function make_statue_mask (size)
    -- Set up a new image surface on which to drawn the mask shape.
    local surface = Cairo.image_surface_create("argb32", size, size)
    local cr = Cairo.context_create(surface)

    -- Draw an n-pointed star.
    local n = 7                     -- number of points
    local c = size / 2              -- center x/y position
    local r1, r2 = c - 2, c * .6    -- outer and inner radii
    local angstep = (2 * PI) / n
    for angmul = 1, n do
        local ang1 = angmul * angstep
        local ang2 = ang1 + angstep / 2
        cr:line_to(c + r1 * math.cos(ang1), c + r1 * math.sin(ang1))
        cr:line_to(c + r2 * math.cos(ang2), c + r2 * math.sin(ang2))
    end
    cr:close_path()

    cr:set_source_rgba(0, 0, 0, 1)
    cr:fill()

    return surface
end

local surface = Cairo.image_surface_create("rgb24", IMG_WD, IMG_HT)
local cr = Cairo.context_create(surface)

local bronzepic = load_jpeg("examples/images/bronze.jpg")
local flowerpic = load_jpeg("examples/images/flowers.jpg")
local snowpic   = load_jpeg("examples/images/snow.jpg")

-- Show the snowy scene at the bottom of the picture, faded out at the top,
-- using a linear gradient as the mask.
cr:set_source(snowpic, 0, IMG_HT - snowpic:get_height())
cr:mask(make_snow_gradient())

-- Show the flowers in a faded circle, with the image scaled to half size,
-- by masking with a radial gradient.
cr:scale(0.5, 0.5)
cr:set_source(flowerpic, 0, 0)
cr:identity_matrix()
cr:mask(make_flower_gradient(flowerpic:get_height() / 4))

-- Show the bronze statue in a sharply defined cut-out shape, by drawing the
-- shape onto a blank transparent surface first, and then using that surface
-- as the mask.
cr:set_source(bronzepic, 180, 30)
cr:mask(make_statue_mask(200), 200, 0)

surface:write_to_png("masking.png")

-- vi:ts=4 sw=4 expandtab
