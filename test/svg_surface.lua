require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.svg_surface", lunit.testcase, package.seeall)

teardown = clean_up_temp_files

if Cairo.HAS_SVG_SURFACE then
    function test_svg_versions ()
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

    function test_create ()
        local filename = tmpname()
        local surface = Cairo.svg_surface_create(filename, 300, 200)
        check_svg_surface(surface)
        draw_arbitrary_stuff(Cairo, surface)
        surface:finish()
        check_file_contains_svg(filename)
    end

    function test_create_stream ()
        local filename = tmpname()
        local fh = assert(io.open(filename, "wb"))
        local surface = Cairo.svg_surface_create(fh, 300, 200)
        check_svg_surface(surface)
        draw_arbitrary_stuff(Cairo, surface)
        surface:finish()
        fh:close()
        check_file_contains_svg(filename)
    end

    function test_create_bad ()
        assert_error("wrong type instead of file/filename",
                     function () Cairo.svg_surface_create(true, 300, 200) end)
    end
end

-- vi:ts=4 sw=4 expandtab
