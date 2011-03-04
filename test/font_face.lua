require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.font_face", lunit.testcase, package.seeall)

function test_double_gc ()
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    local cr = Cairo.context_create(surface)
    local face = cr:get_font_face()
    face:__gc()
    face:__gc()
end

function test_select_get_set_font_face ()
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    local cr = Cairo.context_create(surface)

    cr:select_font_face("sans", "normal", "normal")
    local sans = cr:get_font_face()
    assert_equal(nil, sans:status(), "Error status on font face")
    assert_userdata(sans)
    assert_equal("cairo font face object", sans._NAME)
    assert_equal("toy", sans:get_type())

    cr:select_font_face("serif", "italic", "bold")
    local serif = cr:get_font_face()
    cr:set_font_face(sans)
    assert_equal(sans, cr:get_font_face())
    cr:set_font_face(serif)
    assert_equal(serif, cr:get_font_face())
end

function test_select_font_face ()
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    local cr = Cairo.context_create(surface)
    cr:select_font_face("sans", "italic", "bold")
    if Cairo.toy_font_face_create then
        local font = cr:get_font_face()
        assert_equal("sans", font:get_family())
        assert_equal("italic", font:get_slant())
        assert_equal("bold", font:get_weight())
    end

    cr:select_font_face("serif", "italic")
    if Cairo.toy_font_face_create then
        local font = cr:get_font_face()
        assert_equal("serif", font:get_family())
        assert_equal("italic", font:get_slant())
        assert_equal("normal", font:get_weight())
    end

    cr:select_font_face("serif", nil, "bold")
    if Cairo.toy_font_face_create then
        local font = cr:get_font_face()
        assert_equal("serif", font:get_family())
        assert_equal("normal", font:get_slant())
        assert_equal("bold", font:get_weight())
    end

    cr:select_font_face("serif")
    if Cairo.toy_font_face_create then
        local font = cr:get_font_face()
        assert_equal("serif", font:get_family())
        assert_equal("normal", font:get_slant())
        assert_equal("normal", font:get_weight())
    end

    assert_error("bad family name",
                 function () cr:select_font_face({}, "normal", "normal") end)
    assert_error("bad slant value",
                 function () cr:select_font_face("sans", "foo", "normal") end)
    assert_error("bad weight value",
                 function () cr:select_font_face("sans", "normal", "foo") end)
end

function test_toy_create_and_accessors ()
    if Cairo.toy_font_face_create then
        local font = Cairo.toy_font_face_create("serif", "italic", "bold")
        assert_userdata(font)
        assert_equal("cairo font face object", font._NAME)
        assert_equal("toy", font:get_type())
        assert_equal("serif", font:get_family())
        assert_equal("italic", font:get_slant())
        assert_equal("bold", font:get_weight())

        -- Two styling options should default to 'normal'.
        font = Cairo.toy_font_face_create("serif", "italic")
        assert_equal("italic", font:get_slant())
        assert_equal("normal", font:get_weight())
        font = Cairo.toy_font_face_create("serif")
        assert_equal("normal", font:get_slant())
        assert_equal("normal", font:get_weight())
    else
        local surface = Cairo.image_surface_create("rgb24", 23, 45)
        local font = Cairo.context_create(surface):get_font_face()
        assert_nil(Cairo.toy_font_face_create)
        assert_nil(font.get_family)
        assert_nil(font.get_slant)
        assert_nil(font.get_weight)
    end
end

function test_user_font ()
    if Cairo.user_font_face_create then
        -- Should work with only one callback defined.
        local font = Cairo.user_font_face_create({
            render_glyph = function() end,
        })
        assert_userdata(font)
        assert_equal("cairo font face object", font._NAME)
        assert_equal("user", font:get_type())

        -- Take this oppurtunity to test that certain methods don't work on
        -- non-toy font face objects.
        assert_error("get_family", function () font:get_family() end)
        assert_error("get_slant",  function () font:get_slant()  end)
        assert_error("get_weight", function () font:get_weight() end)

        -- Test all callbacks are called right.
        local called_callbacks = {}
        local init = function (font, cr, extents)
            if not called_callbacks.init then
                assert_equal("cairo scaled font object", font._NAME)
                assert_equal("cairo context object", cr._NAME)
                assert_table(extents)
                assert_number(extents.ascent)
            end
            called_callbacks.init = true
        end
        local render_glyph = function (font, glyph, cr, extents)
            if not called_callbacks.render_glyph then
                assert_equal("cairo scaled font object", font._NAME)
                assert_equal(string.byte("x"), glyph)
                assert_equal("cairo context object", cr._NAME)
                assert_table(extents)
                assert_number(extents.x_bearing)
            end
            called_callbacks.render_glyph = true
            extents.x_advance = 1.5
        end
        local text_to_glyphs = function (font, utf8, want_clusters)
            if not called_callbacks.text_to_glyphs then
                assert_equal("cairo scaled font object", font._NAME)
                assert_string(utf8)
                assert_boolean(want_clusters)
            end
            called_callbacks.text_to_glyphs = true
            return nil      -- no glyph info provided here
        end
        local unicode_to_glyph = function (font, unicode)
            if not called_callbacks.unicode_to_glyph then
                assert_equal("cairo scaled font object", font._NAME)
                assert_number(unicode)
                assert(unicode > 0)
            end
            called_callbacks.unicode_to_glyph = true
            return unicode
        end

        local surface = Cairo.image_surface_create("rgb24", 50, 50)
        local cr = Cairo.context_create(surface)

        cr:set_font_face(Cairo.user_font_face_create({
            init = init,
            render_glyph = render_glyph,
            unicode_to_glyph = unicode_to_glyph,
        }))
        cr:show_text("x")
        cr:set_font_face(Cairo.user_font_face_create({
            init = init,
            render_glyph = render_glyph,
            text_to_glyphs = text_to_glyphs,
        }))
        cr:show_text("x")

        assert_true(called_callbacks.init)
        assert_true(called_callbacks.render_glyph)
        assert_true(called_callbacks.text_to_glyphs)
        assert_true(called_callbacks.unicode_to_glyph)
    else
        assert_nil(Cairo.user_font_face_create)
    end
end

function test_user_font_providing_text_clusters ()
    if Cairo.user_font_face_create and Cairo.HAS_PDF_SURFACE then
        local function text_to_glyphs (font, text, want_clusters)
            assert_true(want_clusters)
            return { { 123, 10, 20 }, { 321, 11, 22 } },    -- glyphs
                   { { 0, 1 }, { 1, 1 } }                   -- text clusters
        end
        local filename = tmpname()
        local surface = Cairo.pdf_surface_create(filename, 50, 50)
        local cr = Cairo.context_create(surface)

        cr:set_font_face(Cairo.user_font_face_create({
            render_glyph = function() end,
            text_to_glyphs = text_to_glyphs,
        }))
        cr:show_text("x")
    end
end

function test_user_font_err ()
    if Cairo.user_font_face_create then
        -- Options should be provided in a table.
        local ok, err = pcall(Cairo.user_font_face_create, "foo")
        assert_false(ok)
        assert_match("table", err)

        -- Missing the one mandatory callback.
        ok, err = pcall(Cairo.user_font_face_create, {})
        assert_false(ok)
        assert_match("the 'render_glyph' option is required", err)

        -- Callbacks which aren't function values.
        for _, func in ipairs{
            "init", "render_glyph", "text_to_glyphs", "unicode_to_glyph",
        } do
            local options = { render_glyph = function () end }
            options[func] = "not a function"
            ok, err = pcall(Cairo.user_font_face_create, options)
            assert_false(ok)
            assert_match("the '" .. func .. "' option.*function", err)
        end
    end
end

function test_equality ()
    -- Different userdatas, same Cairo object.
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    local cr = Cairo.context_create(surface)
    local font1 = cr:get_font_face()
    local font2 = cr:get_font_face()
    assert_true(font1 == font2)

    -- Different userdatas, different Cairo objects.
    cr:select_font_face("serif")
    local font3 = cr:get_font_face()
    assert_false(font1 == font3)
end

-- vi:ts=4 sw=4 expandtab
