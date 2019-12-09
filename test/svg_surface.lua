require "test-setup"
local lunit = require "lunit"
local Cairo = require "oocairo"

local assert_error      = lunit.assert_error
local assert_true       = lunit.assert_true
local assert_false      = lunit.assert_false
local assert_equal      = lunit.assert_equal
local assert_userdata   = lunit.assert_userdata
local assert_table      = lunit.assert_table
local assert_number     = lunit.assert_number
local assert_match      = lunit.assert_match
local assert_string     = lunit.assert_string
local assert_boolean    = lunit.assert_boolean
local assert_not_equal  = lunit.assert_not_equal
local assert_nil        = lunit.assert_nil

local module = { _NAME="test.svg_surface" }

module.teardown = clean_up_temp_files

if Cairo.HAS_SVG_SURFACE then
    function module.test_svg_versions ()
        local versions = Cairo.svg_get_versions()
        assert_table(versions)
        for k, v in pairs(versions) do
            assert_number(k)
            assert_string(v)
        end
    end

    local function check_svg_surface (surface)
        assert_userdata(surface)
        assert_equal("cairo surface object", surface._NAME)
        assert_equal("svg", surface:get_type())
    end

    local function check_file_contains_svg (filename)
        local fh = assert(io.open(filename, "rb"))
        local data = fh:read("*a")
        fh:close()
        assert_match("<svg", data)
    end

    function module.test_create ()
        local filename = tmpname()
        local surface = Cairo.svg_surface_create(filename, 300, 200)
        check_svg_surface(surface)
        draw_arbitrary_stuff(Cairo, surface)
        surface:finish()
        check_file_contains_svg(filename)
    end

    function module.test_create_stream ()
        local filename = tmpname()
        local fh = assert(io.open(filename, "wb"))
        local surface = Cairo.svg_surface_create(fh, 300, 200)
        check_svg_surface(surface)
        draw_arbitrary_stuff(Cairo, surface)
        surface:finish()
        fh:close()
        check_file_contains_svg(filename)
    end

    function module.test_create_bad ()
        assert_error("wrong type instead of file/filename",
                     function () Cairo.svg_surface_create(true, 300, 200) end)
    end
end

lunit.testcase(module)
return module

-- vi:ts=4 sw=4 expandtab
