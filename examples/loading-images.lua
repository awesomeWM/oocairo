-- Demonstrate loading a PNG image three different ways.  The third will
-- be skipped unless my 'memoryfile' module is installed, so the output
-- image will have either two or three copies of the wood pattern.

local Cairo = require "oocairo"

local PIC_FILENAME = "examples/images/wood1.png"
local PIC_WD, PIC_HT = 96, 96

local MARGIN = 10
local IMG_WD, IMG_HT = PIC_WD * 3 + MARGIN * 4, PIC_HT + MARGIN * 2

-- These three functions do the same thing (loading a PNG file and creating
-- a surface object from it), but in different ways.  Loading directly from
-- a filename is convenient, but using a file handle is more flexible.
-- Loading from a string allows images from other libraries like GD to be
-- converted into Cairo objects without going through a temporary file.
local function load_pic_from_filename ()
    return Cairo.image_surface_create_from_png(PIC_FILENAME)
end

local function load_pic_from_file_handle ()
    local fh = assert(io.open(PIC_FILENAME, "rb"))
    local surface = Cairo.image_surface_create_from_png(fh)
    fh:close()
    return surface
end

-- This one only works if the 'memoryfile' module is installed.
local function load_pic_from_string ()
    local ok, MemFile = pcall(require, "memoryfile")
    if ok then
        local fh = assert(io.open(PIC_FILENAME, "rb"))
        local picdata = fh:read("*a")
        fh:close()
        fh = MemFile.open(picdata)
        return Cairo.image_surface_create_from_png(fh)
    end
end

local surface = Cairo.image_surface_create("rgb24", IMG_WD, IMG_HT)
local cr = Cairo.context_create(surface)

for i, func in ipairs{ load_pic_from_filename,
                       load_pic_from_file_handle,
                       load_pic_from_string }
do
    local surface = func()
    if surface then
        cr:set_source(surface, MARGIN * i + PIC_WD * (i - 1), MARGIN)
        cr:paint()
    end
end

surface:write_to_png("loading-images.png")

-- vi:ts=4 sw=4 expandtab
