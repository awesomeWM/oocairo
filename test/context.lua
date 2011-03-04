require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.context", lunit.testcase, package.seeall)

local PI = 2 * math.asin(1)
local EPSILON = 0.000001

local function assert_about_equal (expected, got, msg)
    assert(got > (expected - EPSILON) and got < (expected + EPSILON), msg)
end

function setup ()
    surface = Cairo.image_surface_create("rgb24", 23, 45)
    cr = Cairo.context_create(surface)
end
function teardown ()
    assert_equal(nil, cr:status(), "Error status on context")
    assert_equal(nil, surface:status(), "Error status on surface")
    surface = nil
    cr = nil
end

function test_double_gc ()
    local cr = Cairo.context_create(surface)
    cr:__gc()
    cr:__gc()
end

function test_antialias ()
    assert_error("bad value", function () cr:set_antialias("foo") end)
    assert_error("missing value", function () cr:set_antialias(nil) end)
    assert_equal("default", cr:get_antialias(), "default intact after error")
    for _, v in ipairs({ "default", "none", "gray", "subpixel" }) do
        cr:set_antialias(v)
        assert_equal(v, cr:get_antialias())
    end

    -- Boolean values also select reasonable values.
    cr:set_antialias(false)
    assert_equal("none", cr:get_antialias())
    cr:set_antialias(true)
    assert_equal("default", cr:get_antialias())
end

function test_dash ()
    for i, dashpat in ipairs{ {}, {3}, {3,6,9} } do
        for offset = 0, #dashpat do
            local msg = "dash pattern " .. i .. ", offset " .. offset
            cr:set_dash(dashpat, offset)
            local newpat, newoffset = cr:get_dash()
            assert_equal(#dashpat, #newpat, msg .. ", pattern length")
            for j, v in ipairs(dashpat) do
                assert_equal(v, newpat[j], msg .. ", pattern value " .. j)
            end
            assert_equal(offset, newoffset, msg .. ", offset")
        end
    end
end

function test_dash_bad ()
    assert_error("missing offset type", function () cr:set_dash({}, nil) end)
    assert_error("missing pattern", function () cr:set_dash(nil, 0) end)
    assert_error("bad offset type", function () cr:set_dash({}, "foo") end)
    assert_error("bad dash type", function () cr:set_dash({"foo"}, 1) end)
    assert_error("bad dash value", function () cr:set_dash({-1}, 1) end)
    assert_error("bad dash total value", function () cr:set_dash({0,0}, 1) end)
    local pat, offset = cr:get_dash()
    assert_equal(0, #pat, "default pattern intact after errors")
    assert_equal(0, offset, "default offset intact after errors")
end

function test_fill_rule ()
    assert_error("bad value", function () cr:set_fill_rule("foo") end)
    assert_error("missing value", function () cr:set_fill_rule(nil) end)
    assert_equal("winding", cr:get_fill_rule(), "default intact after error")
    for _, v in ipairs({ "winding", "even-odd" }) do
        cr:set_fill_rule(v)
        assert_equal(v, cr:get_fill_rule())
    end
end

function test_line_cap ()
    assert_error("bad value", function () cr:set_line_cap("foo") end)
    assert_error("missing value", function () cr:set_line_cap(nil) end)
    assert_equal("butt", cr:get_line_cap(), "default intact after error")
    for _, v in ipairs({ "butt", "round", "square" }) do
        cr:set_line_cap(v)
        assert_equal(v, cr:get_line_cap())
    end
end

function test_line_join ()
    assert_error("bad value", function () cr:set_line_join("foo") end)
    assert_error("missing value", function () cr:set_line_join(nil) end)
    assert_equal("miter", cr:get_line_join(), "default intact after error")
    for _, v in ipairs({ "miter", "round", "bevel" }) do
        cr:set_line_join(v)
        assert_equal(v, cr:get_line_join())
    end
end

function test_line_width ()
    assert_error("bad type", function () cr:set_line_width("foo") end)
    assert_error("missing value", function () cr:set_line_width(nil) end)
    assert_error("negative width", function () cr:set_line_width(-3) end)
    assert_equal(2, cr:get_line_width(), "default intact after error")
    for _, v in ipairs({ 0, 1, 2, 23.5 }) do
        cr:set_line_width(v)
        assert_equal(v, cr:get_line_width())
    end
end

function test_miter_limit ()
    assert_error("bad type", function () cr:set_miter_limit("foo") end)
    assert_error("missing value", function () cr:set_miter_limit(nil) end)
    assert_equal(10, cr:get_miter_limit(), "default intact after error")
    for _, v in ipairs({ 0, 1, 2, 23.5 }) do
        cr:set_miter_limit(v)
        assert_equal(v, cr:get_miter_limit())
    end
end

function test_operator ()
    assert_error("bad value", function () cr:set_operator("foo") end)
    assert_error("missing value", function () cr:set_operator(nil) end)
    assert_equal("over", cr:get_operator(), "default intact after error")
    for _, v in ipairs({
        "clear",
        "source", "over", "in", "out", "atop",
        "dest", "dest-over", "dest-in", "dest-out", "dest-atop",
        "xor", "add", "saturate",
    }) do
        cr:set_operator(v)
        assert_equal(v, cr:get_operator())
    end
end

function test_save_restore ()
    cr:save()
    cr:set_line_width(3)
    cr:save()
    cr:set_line_width(4)
    cr:restore()
    assert_equal(3, cr:get_line_width())
    cr:restore()
    assert_equal(2, cr:get_line_width())
end

function test_source_rgb ()
    cr:set_source_rgb(0.1, 0.2, 0.3)
    assert_error("not enough args", function () cr:set_source_rgb(0.1, 0.2) end)
    assert_error("bad arg type 1", function () cr:set_source_rgb("x", 0, 0) end)
    assert_error("bad arg type 2", function () cr:set_source_rgb(0, "x", 0) end)
    assert_error("bad arg type 3", function () cr:set_source_rgb(0, 0, "x") end)
end

function test_source_rgba ()
    cr:set_source_rgba(0.1, 0.2, 0.3, 0.4)
    assert_error("not enough args",
                 function () cr:set_source_rgba(0.1, 0.2, 0.3) end)
    assert_error("bad arg type 1",
                 function () cr:set_source_rgba("x", 0, 0, 0) end)
    assert_error("bad arg type 2",
                 function () cr:set_source_rgba(0, "x", 0, 0) end)
    assert_error("bad arg type 3",
                 function () cr:set_source_rgba(0, 0, "x", 0) end)
    assert_error("bad arg type 4",
                 function () cr:set_source_rgba(0, 0, 0, "x") end)
end

function test_source_gdk_color ()
    local c = {}
    if pcall(require, "gtk") then c = gtk.new"GdkColor" end
    c.red = 0
    c.green = 0x7FFF
    c.blue = 0xFFFF

    -- no alpha
    cr:set_source_rgba(.3, .3, .3, .3)
    cr:set_source_gdk_color(c)
    local r, g, b, a = cr:get_source():get_rgba()
    assert_equal(0, r)
    assert(g > 0.499 and g < 0.501)
    assert_equal(1, b)
    assert_equal(1, a)

    -- include alpha
    cr:set_source_rgba(.3, .3, .3, .3)
    cr:set_source_gdk_color(c, 0x3FFF)
    r, g, b, a = cr:get_source():get_rgba()
    assert_equal(0, r)
    assert(g > 0.499 and g < 0.501)
    assert_equal(1, b)
    assert(a > 0.249 and a < 0.251)
end

function test_source ()
    local src = Cairo.pattern_create_rgb(0.25, 0.5, 0.75)
    cr:set_source(src)
    local gotsrc = cr:get_source()
    assert_userdata(gotsrc)
    assert_equal("cairo pattern object", gotsrc._NAME)
    assert_equal("solid", gotsrc:get_type())
    local r, g, b = gotsrc:get_rgba()
    assert_equal(0.25, r)
    assert_equal(0.5, g)
    assert_equal(0.75, b)
end

function test_target ()
    local targ = cr:get_target()
    assert_userdata(targ)
    assert_equal("cairo surface object", targ._NAME)
end

function test_group_target ()
    cr:push_group()
    local targ = cr:get_group_target()
    assert_userdata(targ)
    assert_equal("cairo surface object", targ._NAME)
end

function test_pop_group ()
    cr:push_group()
    local pat = cr:pop_group()
    assert_userdata(pat)
    assert_equal("cairo pattern object", pat._NAME)
end

function test_tolerance ()
    assert_error("bad type", function () cr:set_tolerance("foo") end)
    assert_error("missing value", function () cr:set_tolerance(nil) end)
    assert_equal(0.1, cr:get_tolerance(), "default intact after error")
    for _, v in ipairs({ 0.05, 1, 2, 23.5 }) do
        cr:set_tolerance(v)
        assert_equal(v, cr:get_tolerance())
    end
end

function test_transform ()
    cr:translate(10, 20)
    local x, y = cr:user_to_device(3, 4)
    assert_equal(13, x)
    assert_equal(24, y)

    cr:scale(10, 20)
    x, y = cr:user_to_device(3, 4)
    assert_equal(40, x)
    assert_equal(100, y)
    x, y = cr:user_to_device_distance(3, 4)
    assert_equal(30, x)
    assert_equal(80, y)
    x, y = cr:device_to_user(40, 100)
    assert_equal(3, x)
    assert_equal(4, y)
    x, y = cr:device_to_user_distance(30, 80)
    assert_equal(3, x)
    assert_equal(4, y)

    cr:identity_matrix()
    x, y = cr:user_to_device(3, 4)
    assert_equal(3, x)
    assert_equal(4, y)

    cr:identity_matrix()
    cr:rotate(PI / 2)       -- 90 degrees
    x, y = cr:user_to_device(3, 4)
    assert_about_equal(-4, x)
    assert_about_equal(3, y)

    cr:identity_matrix()
    local m = cr:get_matrix()
    assert_table(m)

    m[5] = 10       -- equivalent to translate(10,20)
    m[6] = 20
    cr:set_matrix(m)
    x, y = cr:user_to_device(3, 4)
    assert_equal(13, x)
    assert_equal(24, y)

    cr:identity_matrix()
    cr:translate(10, 20)
    m = Cairo.matrix_create()
    m:scale(10, 20)
    cr:transform(m)
    x, y = cr:user_to_device(3, 4)
    assert_equal(40, x)
    assert_equal(100, y)
end

function test_text_extents ()
    check_text_extents(cr:text_extents("foo bar quux"))
end

function test_glyph_extents ()
    local glyphs = { {73,10,20}, {82,30,40}, {91,50,60} }
    check_text_extents(cr:glyph_extents(glyphs))
end

function test_font_extents ()
    check_font_extents(cr:font_extents())
end

function test_font_options ()
    local origopt = cr:get_font_options()
    assert_userdata(origopt)
    assert_equal("cairo font options object", origopt._NAME)
    assert_equal("default", origopt:get_antialias())

    local newopt = Cairo.font_options_create()
    newopt:set_antialias("none")
    cr:set_font_options(newopt)
    local adjusted = cr:get_font_options()
    assert_equal("none", adjusted:get_antialias())
end

-- vi:ts=4 sw=4 expandtab
