require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.scaled_font", lunit.testcase, package.seeall)

local function mk_surface_cr ()
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    local cr = Cairo.context_create(surface)
    return surface, cr
end

local function mk_scaled_font (face, size, ...)
    local font_mat = Cairo.matrix_create()
    font_mat:scale(size, size)
    local ctm = Cairo.matrix_create()
    return Cairo.scaled_font_create(face, font_mat, ctm, ...)
end

function test_double_gc ()
    local _, cr = mk_surface_cr()
    local font = cr:get_scaled_font()
    font:__gc()
    font:__gc()
end

function test_create ()
    local surface, cr = mk_surface_cr()
    cr:select_font_face("sans", "normal", "normal")
    local face = cr:get_font_face()
    local font = mk_scaled_font(face, 23)
    assert_userdata(font)
    assert_equal("cairo scaled font object", font._NAME)
    assert_equal("cairo font face object", font:get_font_face()._NAME)
    assert_equal(nil, font:status(), "Error status on scaled font")
    assert_string(font:get_type())
end

function test_get_set_on_context ()
    local surface, cr = mk_surface_cr()
    local origfont = cr:get_scaled_font()
    assert_userdata(origfont)
    assert_equal("cairo scaled font object", origfont._NAME)

    local newfont = mk_scaled_font(cr:get_font_face(), 23)
    cr:set_scaled_font(newfont)
    assert_true(cr:get_scaled_font() ~= origfont)
end

function test_font_extents ()
    local surface, cr = mk_surface_cr()
    local font = mk_scaled_font(cr:get_font_face(), 23)
    check_font_extents(font:extents())
end

function test_text_extents ()
    local surface, cr = mk_surface_cr()
    local font = mk_scaled_font(cr:get_font_face(), 23)
    check_text_extents(font:text_extents("foo bar quux"))
end

function test_glyph_extents ()
    local surface, cr = mk_surface_cr()
    local font = mk_scaled_font(cr:get_font_face(), 23)
    local x, y = 10, 20
    local glyphs = { {73,10,20}, {82,30,40}, {91,50,60} }
    check_text_extents(font:glyph_extents(glyphs))
end

function test_matrix ()
    local surface, cr = mk_surface_cr()
    local font = mk_scaled_font(cr:get_font_face(), 23)

    local m = font:get_font_matrix()
    check_matrix_elems(m)
    assert_equal("cairo matrix object", m._NAME)

    m = font:get_ctm()
    check_matrix_elems(m)
    assert_equal("cairo matrix object", m._NAME)

    if font.get_scale_matrix then
        m = font:get_scale_matrix()
        check_matrix_elems(m)
        assert_equal("cairo matrix object", m._NAME)
    end
end

function test_text_to_glyphs ()
    local surface, cr = mk_surface_cr()
    local font = mk_scaled_font(cr:get_font_face(), 23)

    if font.text_to_glyphs then
        local glyphs, clusters = font:text_to_glyphs(10, 100, "foo")

        assert_table(glyphs)
        assert_equal(3, #glyphs)
        for _, v in ipairs(glyphs) do
            assert_table(v)
            assert_equal(3, #v)
            assert_number(v[1])
            assert_number(v[2])
            assert_number(v[3])
            assert_equal(100, v[3])
        end

        assert_table(clusters)
        assert_equal(3, #clusters)
        assert_false(clusters.backward)
        for _, v in ipairs(clusters) do
            assert_table(v)
            assert_equal(2, #v)
            assert_number(v[1])
            assert_equal(1, v[1])
            assert_number(v[2])
            assert_equal(1, v[2])
        end
    end
end

function test_font_options ()
    local origopt = Cairo.font_options_create()
    origopt:set_antialias("gray")
    origopt:set_subpixel_order("vbgr")
    local _, cr = mk_surface_cr()
    local font = mk_scaled_font(cr:get_font_face(), 23, origopt)

    local gotopt = font:get_font_options()
    assert_userdata(gotopt)
    assert_equal("cairo font options object", gotopt._NAME)
    assert_equal("gray", gotopt:get_antialias())
    assert_equal("vbgr", gotopt:get_subpixel_order())
    assert_true(gotopt == origopt)
    assert_equal(nil, font:status(), "Error status on scaled font")
    assert_equal(nil, gotopt:status(), "Error status on font options")
end

-- vi:ts=4 sw=4 expandtab
