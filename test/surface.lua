require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.surface", lunit.testcase, package.seeall)

-- Some tests use this, but it isn't essential, so they will be skipped if
-- the 'memoryfile' module isn't installed.
local MemFile
do
    local ok
    ok, mod = pcall(require, "memoryfile")
    if ok then MemFile = mod end
end

teardown = clean_up_temp_files

local FILENAME = (srcdir or ".") .. "/examples/images/pattern.png"
local WIDTH, HEIGHT = 15, 10

local function check_image_surface (surface, desc)
    assert_userdata(surface, desc .. ", userdata")
    assert_equal("cairo surface object", surface._NAME, desc .. ", mt name")
    assert_equal("image", surface:get_type(), desc .. ", type")
    if surface.has_show_text_glyphs then
        assert_false(surface:has_show_text_glyphs())
    end
end

function test_image_surface_create ()
    for format, content in pairs({
        rgb24 = "color", argb32 = "color-alpha",
        a8 = "alpha", a1 = "alpha"
    }) do
        local surface = Cairo.image_surface_create(format, 23, 45)
        check_image_surface(surface, "format " .. format)
        assert_equal(format, surface:get_format())
        assert_equal(content, surface:get_content(), "content for " .. format)
        local wd, ht = surface:get_width(), surface:get_height()
        assert_equal(23, wd, "width for " .. format)
        assert_equal(45, ht, "height for " .. format)
    end
end

function test_double_gc ()
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    surface:__gc()
    surface:__gc()
end

function test_image_surface_create_bad ()
    assert_error("bad format", function ()
        Cairo.image_surface_create("foo", 23, 45)
    end)
    assert_error("bad width type", function ()
        Cairo.image_surface_create("rgb24", "x", 23)
    end)
    assert_error("negative width value", function ()
        Cairo.image_surface_create("rgb24", -23, 45)
    end)
    assert_error("bad height type", function ()
        Cairo.image_surface_create("rgb24", 23, "x")
    end)
    assert_error("negative height value", function ()
        Cairo.image_surface_create("rgb24", 23, -45)
    end)
end

function test_surface_create_similar ()
    local base = assert(Cairo.image_surface_create("rgb24", 23, 45))
    for _, v in ipairs({ "color", "alpha", "color-alpha" }) do
        local surface = Cairo.surface_create_similar(base, v, 23, 45)
        assert_userdata(surface, "got userdata for " .. v)
        assert_equal("cairo surface object", surface._NAME,
                     "got surface object for " .. v)
        assert_equal(v, surface:get_content(), "right content")
    end
end

function test_surface_create_similar_bad ()
    local base = assert(Cairo.image_surface_create("rgb24", 23, 45))
    assert_error("bad format", function ()
        Cairo.surface_create_similar(base, "foo", 23, 45)
    end)
    assert_error("bad width type", function ()
        Cairo.surface_create_similar(base, "color", "x", 23)
    end)
    assert_error("negative width value", function ()
        Cairo.surface_create_similar(base, "color", -23, 45)
    end)
    assert_error("bad height type", function ()
        Cairo.surface_create_similar(base, "color", 23, "x")
    end)
    assert_error("negative height value", function ()
        Cairo.surface_create_similar(base, "color", 23, -45)
    end)
end

function test_device_offset ()
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    local x, y = surface:get_device_offset()
    assert_equal(0, x)
    assert_equal(0, y)
    surface:set_device_offset(-5, 3.2)
    x, y = surface:get_device_offset()
    assert_equal(-5, x)
    assert_equal(3.2, y)
end

function test_fallback_resolution ()
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    if surface.get_fallback_resolution then
        local x, y = surface:get_fallback_resolution()
        assert_equal(300, x)
        assert_equal(300, y)
        surface:set_fallback_resolution(123, 456)
        x, y = surface:get_fallback_resolution()
        assert_equal(123, x)
        assert_equal(456, y)
    else
        assert_nil(surface.get_fallback_resolution)
    end
end

if Cairo.HAS_SVG_SURFACE then
    function test_not_image_surface ()
        local surface = Cairo.svg_surface_create(tmpname(), 300, 200)
        assert_error("get_width on non-image surface",
                     function () surface:get_width() end)
        assert_error("get_height on non-image surface",
                     function () surface:get_height() end)
        assert_error("get_format on non-image surface",
                     function () surface:get_format() end)
        assert_nil(surface:get_data(), "get_data on non-image surface")
    end
end

function test_not_pdf_or_ps_surface ()
    local surface = Cairo.image_surface_create("rgb24", 30, 20)
    assert_error("set_size on non-PDF or PostScript surface",
                 function () surface:set_size(40, 50) end)
end

if Cairo.HAS_PS_SURFACE then
    function test_not_ps_surface ()
        local surface = Cairo.image_surface_create("rgb24", 30, 20)
        assert_error("get_eps on non-PS surface",
                     function () surface:get_eps() end)
        assert_error("set_eps on non-PS surface",
                     function () surface:set_eps(true) end)
    end
end

local function check_wood_image_surface (surface)
    check_image_surface(surface, "load PNG from filename")
    assert_equal(WIDTH, surface:get_width())
    assert_equal(HEIGHT, surface:get_height())
end

if Cairo.HAS_PNG_FUNCTIONS then
    function test_create_from_png ()
        local surface = Cairo.image_surface_create_from_png(FILENAME)
        check_wood_image_surface(surface)
    end

    function test_create_from_png_error ()
        assert_error("trying to load PNG file which doesn't exist", function ()
            Cairo.image_surface_create_from_png("nonexistent-file.png")
        end)
        assert_error("wrong type instead of file/filename", function ()
            Cairo.image_surface_create_from_png(false)
        end)
    end

    function test_create_from_png_stream ()
        local fh = assert(io.open(FILENAME, "rb"))
        local surface = Cairo.image_surface_create_from_png(fh)
        fh:close()
        check_wood_image_surface(surface)
    end
end

if MemFile and Cairo.HAS_PNG_FUNCTIONS then
    function test_create_from_png_string ()
        local fh = assert(io.open(FILENAME, "rb"))
        local data = fh:read("*a")
        fh:close()
        fh = MemFile.open(data)
        local surface = Cairo.image_surface_create_from_png(fh)
        fh:close()
        check_wood_image_surface(surface)
    end
end

local function check_data_is_png (data)
    assert_match("^\137PNG\13\10", data)
end
local function check_file_contains_png (filename)
    local fh = assert(io.open(filename, "rb"))
    local data = fh:read("*a")
    fh:close()
    check_data_is_png(data)
end

if Cairo.HAS_PNG_FUNCTIONS then
    function test_write_to_png ()
        local surface = Cairo.image_surface_create("rgb24", 23, 45)
        local filename = tmpname()
        surface:write_to_png(filename)
        check_file_contains_png(filename)
    end

    function test_write_to_png_stream ()
        local surface = Cairo.image_surface_create("rgb24", 23, 45)
        local filename = tmpname()
        local fh = assert(io.open(filename, "wb"))
        surface:write_to_png(fh)
        fh:close()
        check_file_contains_png(filename)
    end
end

if MemFile and Cairo.HAS_PNG_FUNCTIONS then
    function test_write_to_png_string ()
        local surface = Cairo.image_surface_create("rgb24", 23, 45)
        local fh = MemFile.open()
        surface:write_to_png(fh)
        check_data_is_png(tostring(fh))
        fh:close()
    end
end

function test_font_options ()
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    local opt = surface:get_font_options()
    assert_userdata(opt)
    assert_equal("cairo font options object", opt._NAME)
    assert_equal("default", opt:get_antialias())
end

function test_format_stride_for_width ()
    -- Check the minimum values, because depending on platform there might
    -- be extra bytes for alignment.
    assert(Cairo.format_stride_for_width("a1", 10) >= 4)
    assert(Cairo.format_stride_for_width("a8", 10) >= 10)
    assert(Cairo.format_stride_for_width("rgb24", 10) >= 40)
    assert(Cairo.format_stride_for_width("argb32", 10) >= 40)
end

function test_byte_order ()
    assert_string(Cairo.BYTE_ORDER)
    assert("argb" == Cairo.BYTE_ORDER or "bgra" == Cairo.BYTE_ORDER)
end

local function check_pixel_in_data(data, stride, x, y, a, r, g, b)
    local offset = y * stride + x * 4
    local got_a, got_r, got_g, got_b
    if Cairo.BYTE_ORDER == "argb" then  -- big endian
        got_a, got_r, got_g, got_b = data:byte(offset + 1, offset + 4)
    else                                -- little endian
        got_b, got_g, got_r, got_a = data:byte(offset + 1, offset + 4)
    end

    local msg = "pixel " .. x .. ", " .. y
    assert_equal(a, got_a, msg)
    assert_equal(r, got_r, msg)
    assert_equal(g, got_g, msg)
    assert_equal(b, got_b, msg)
end

function test_get_data ()
    -- Test with a very small image surface, so that we can predict what
    -- each pixel will be.
    local surface = Cairo.image_surface_create("argb32", 3, 2)
    local cr = Cairo.context_create(surface)

    -- These drawing instructions should produce a pattern like this:
    --      R G B   (primary red, green, and blue, all completely opaque)
    --      T T B   (T meaning transparent black)
    cr:set_line_width(1)
    cr:set_source_rgb(1, 0, 0); cr:rectangle(0, 0, 1, 1); cr:fill()
    cr:set_source_rgb(0, 1, 0); cr:rectangle(1, 0, 1, 1); cr:fill()
    cr:set_source_rgb(0, 0, 1); cr:rectangle(2, 0, 1, 2); cr:fill()

    local data, stride = surface:get_data()
    assert(12 == stride or 16 == stride)        -- might be 16 on 64 bit, maybe
    assert_string(data)
    assert_equal(stride * 2, data:len())

    check_pixel_in_data(data, stride, 0, 0, 255, 255, 0, 0)
    check_pixel_in_data(data, stride, 1, 0, 255, 0, 255, 0)
    check_pixel_in_data(data, stride, 2, 0, 255, 0, 0, 255)
    check_pixel_in_data(data, stride, 0, 1, 0, 0, 0, 0)
    check_pixel_in_data(data, stride, 1, 1, 0, 0, 0, 0)
    check_pixel_in_data(data, stride, 2, 1, 255, 0, 0, 255)
end

if Cairo.check_version(1, 10, 0) then
    function test_subsurface()
        local surface = Cairo.image_surface_create("rgb24", 23, 45)
        local sub = surface:create_for_rectangle(10, 11, 12, 13)
        assert_equal("subsurface", sub:get_type())
        assert_error(function() sub:create_for_rectangle(0, 0, -1, -1) end)
    end
end

if pcall(require, "gtk") then
    function test_get_gdk_pixbuf_argb32 ()
        local surface = Cairo.image_surface_create("argb32", 23, 14)
        local pixbuf = surface:get_gdk_pixbuf()
        assert_equal(gtk.GDK_COLORSPACE_RGB, pixbuf:get_colorspace())
        assert_equal(4, pixbuf:get_n_channels())
        assert_equal(true, pixbuf:get_has_alpha())
        assert_equal(8, pixbuf:get_bits_per_sample())
        assert_equal(23, pixbuf:get_width())
        assert_equal(14, pixbuf:get_height())
    end

    function test_get_gdk_pixbuf_rgb24 ()
        local surface = Cairo.image_surface_create("rgb24", 23, 14)
        local pixbuf = surface:get_gdk_pixbuf()
        assert_equal(gtk.GDK_COLORSPACE_RGB, pixbuf:get_colorspace())
        assert_equal(3, pixbuf:get_n_channels())
        assert_equal(false, pixbuf:get_has_alpha())
        assert_equal(8, pixbuf:get_bits_per_sample())
        assert_equal(23, pixbuf:get_width())
        assert_equal(14, pixbuf:get_height())
    end

    function test_get_gdk_pixbuf_a8 ()
        local surface = Cairo.image_surface_create("a8", 23, 14)
        assert_error("GdkPixbuf from A8 image not supported",
                     function () surface:get_gdk_pixbuf() end)
    end
end

function test_equality ()
    -- Create two userdatas containing the same pointer value (different
    -- objects in Lua, but the same objects in C, so should be equal).
    local surface1 = Cairo.image_surface_create("rgb24", 23, 45)
    local cr = Cairo.context_create(surface1)
    local surface2 = cr:get_target()
    assert_true(surface1 == surface2)

    -- Create a new, completely separate object, which should be distinct
    -- from any other.
    local surface3 = Cairo.image_surface_create("rgb24", 23, 45)
    assert_false(surface1 == surface3)
end

if Cairo.HAS_RECORDING_SURFACE then
    function test_recording_surface ()
        local function test(surface, expected_x, expected_y, expected_width, expected_height)
            local x, y, width, height = Cairo.recording_surface_ink_extents(surface)
            assert_equal(0, x)
            assert_equal(0, y)
            assert_equal(0, width)
            assert_equal(0, height)

            local cr = Cairo.context_create(surface)
            cr:set_source_rgb(1, 0, 0)
            cr:paint()

            x, y, width, height = Cairo.recording_surface_ink_extents(surface)
            assert_equal(expected_x, x)
            assert_equal(expected_y, y)
            assert_equal(expected_width, width)
            assert_equal(expected_height, height)
        end
        -- unbounded
        local surface = Cairo.recording_surface_create("color")
        test(surface, -8388608, -8388608, -1, -1)

        -- bounded
        surface = Cairo.recording_surface_create("color-alpha", -10, -10, 20, 20)
        test(surface, -10, -10, 20, 20)

        -- negative size
        assert_error(function() Cairo.recording_surface_create("alpha", 0, 0, -10, -10) end)
    end
end

-- vi:ts=4 sw=4 expandtab
