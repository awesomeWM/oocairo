require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.pdf_surface", lunit.testcase, package.seeall)

teardown = clean_up_temp_files

if Cairo.HAS_PDF_SURFACE then
    local function check_pdf_surface (surface)
        assert_userdata(surface)
        assert_equal("cairo surface object", surface._NAME)
        assert_equal("pdf", surface:get_type())
        if surface.has_show_text_glyphs then
            assert_true(surface:has_show_text_glyphs())
        end
    end

    local function check_file_contains_pdf (filename)
        local fh = assert(io.open(filename, "rb"))
        local data = fh:read("*a")
        fh:close()
        assert_match("^%%PDF", data)
    end

    function test_create ()
        local filename = tmpname()
        local surface = Cairo.pdf_surface_create(filename, 300, 200)
        check_pdf_surface(surface)
        draw_arbitrary_stuff(Cairo, surface)
        surface:finish()
        check_file_contains_pdf(filename)
    end

    function test_create_stream ()
        local filename = tmpname()
        local fh = assert(io.open(filename, "wb"))
        local surface = Cairo.pdf_surface_create(fh, 300, 200)
        check_pdf_surface(surface)
        draw_arbitrary_stuff(Cairo, surface)
        surface:finish()
        fh:close()
        check_file_contains_pdf(filename)
    end

    function test_create_bad ()
        assert_error("wrong type instead of file/filename",
                     function () Cairo.pdf_surface_create(true, 300, 200) end)
    end

    function test_set_size ()
        local surface = Cairo.pdf_surface_create(tmpname(), 300, 200)
        surface:set_size(300, 400)
        draw_arbitrary_stuff(Cairo, surface)
        surface:set_size(500, 600)
        draw_arbitrary_stuff(Cairo, surface)
        surface:finish()
    end

    if Cairo.check_version(1, 10, 0) then
        function test_restrict ()
            local surface = Cairo.pdf_surface_create(tmpname(), 300, 200)
            surface:restrict_to_version("1.4")
            surface:restrict_to_version("1.5")
        end
    end
end

-- vi:ts=4 sw=4 expandtab
