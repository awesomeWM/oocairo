require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.pattern", lunit.testcase, package.seeall)

function test_solid_rgb ()
    local pat = Cairo.pattern_create_rgb(0.25, 0.5, 0.75)
    assert_userdata(pat)
    assert_equal("cairo pattern object", pat._NAME)
    assert_equal("solid", pat:get_type())
    assert_equal(nil, pat:status(), "Error status on pattern")

    local r, g, b, a = pat:get_rgba()
    assert_equal(0.25, r)
    assert_equal(0.5, g)
    assert_equal(0.75, b)
    assert_equal(1, a)
end

function test_solid_rgba ()
    local pat = Cairo.pattern_create_rgba(0.25, 0.5, 0.75, 0.125)
    assert_userdata(pat)
    assert_equal("cairo pattern object", pat._NAME)
    assert_equal("solid", pat:get_type())
    assert_equal(nil, pat:status(), "Error status on pattern")

    local r, g, b, a = pat:get_rgba()
    assert_equal(0.25, r)
    assert_equal(0.5, g)
    assert_equal(0.75, b)
    assert_equal(0.125, a)
end

function test_surface ()
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    local pat = Cairo.pattern_create_for_surface(surface)
    assert_userdata(pat)
    assert_equal("cairo pattern object", pat._NAME)
    assert_equal("surface", pat:get_type())
    assert_equal(nil, pat:status(), "Error status on pattern")

    local gotsurface = pat:get_surface()
    assert_userdata(gotsurface)
    assert_equal("cairo surface object", gotsurface._NAME)
    local wd, ht = gotsurface:get_width(), gotsurface:get_height()
    assert_equal(23, wd)
    assert_equal(45, ht)
end

function test_linear_gradient ()
    local g = Cairo.pattern_create_linear(3, 4, 8, 9)
    assert_userdata(g)
    assert_equal("cairo pattern object", g._NAME)
    assert_equal("linear", g:get_type())
    assert_equal(nil, g:status(), "Error status on pattern")

    local x0, y0, x1, y1 = g:get_linear_points()
    assert_equal(3, x0)
    assert_equal(4, y0)
    assert_equal(8, x1)
    assert_equal(9, y1)
end

function test_radial_gradient ()
    local g = Cairo.pattern_create_radial(3, 4, 5, 8, 9, 10)
    assert_userdata(g)
    assert_equal("cairo pattern object", g._NAME)
    assert_equal(nil, g:status(), "Error status on pattern")
    assert_equal("radial", g:get_type())

    local x0, y0, r0, x1, y1, r1 = g:get_radial_circles()
    assert_equal(3, x0)
    assert_equal(4, y0)
    assert_equal(5, r0)
    assert_equal(8, x1)
    assert_equal(9, y1)
    assert_equal(10, r1)
end

function test_wrong_pattern_type ()
    local pat = Cairo.pattern_create_radial(3, 4, 5, 8, 9, 10)
    assert_error("not linear gradient, get points", function ()
        pat:get_linear_points()
    end)

    pat = Cairo.pattern_create_linear(3, 4, 8, 9)
    assert_error("not radial gradient, get circles", function ()
        pat:get_radial_circles()
    end)

    assert_error("not surface gradient, get surface", function ()
        pat:get_surface()
    end)
end

local function check_color_stop (i, stop, offset, r, g, b, a)
    assert_table(stop, "table for color stop " .. i)
    assert_equal(stop[1], offset, "offset for color stop " .. i)
    assert_equal(stop[2], r, "red for color stop " .. i)
    assert_equal(stop[3], g, "green for color stop " .. i)
    assert_equal(stop[4], b, "blue for color stop " .. i)
    assert_equal(stop[5], a, "alpha for color stop " .. i)
end

function test_add_color_stop ()
    local pat = Cairo.pattern_create_linear(3, 4, 8, 9)
    pat:add_color_stop_rgb(0, 0.25, 0.5, 0.75)
    pat:add_color_stop_rgb(0.125, 1, 0, 1)
    pat:add_color_stop_rgba(0.875, 0, 1, 0, 0.5)
    pat:add_color_stop_rgba(1, 1, 1, 1, 1)

    local stops = pat:get_color_stops()
    assert_table(stops)
    assert_equal(4, #stops)
    check_color_stop(1, stops[1], 0, 0.25, 0.5, 0.75, 1)
    check_color_stop(2, stops[2], 0.125, 1, 0, 1, 1)
    check_color_stop(3, stops[3], 0.875, 0, 1, 0, 0.5)
    check_color_stop(4, stops[4], 1, 1, 1, 1, 1)
end

function test_extend ()
    local pat = Cairo.pattern_create_linear(3, 4, 8, 9)
    assert_error("bad value", function () pat:set_extend("foo") end)
    assert_error("missing value", function () pat:set_extend(nil) end)
    assert_equal("pad", pat:get_extend(), "default intact after error")
    for _, v in ipairs{ "none", "repeat", "reflect", "pad" } do
        pat:set_extend(v)
        assert_equal(v, pat:get_extend())
    end
end

function test_filter ()
    local pat = Cairo.pattern_create_linear(3, 4, 8, 9)
    assert_error("bad value", function () pat:set_filter("foo") end)
    assert_error("missing value", function () pat:set_filter(nil) end)
    assert_equal("good", pat:get_filter(), "default intact after error")
    for _, v in ipairs{
        "fast", "good", "best", "nearest", "bilinear", "gaussian"
    } do
        pat:set_filter(v)
        assert_equal(v, pat:get_filter())
    end
end

function test_not_gradient_pattern ()
    local pat = Cairo.pattern_create_rgb(0.25, 0.5, 0.75)
    assert_error("add_color_stop_rgb on non-gradient pattern",
                 function () pat:add_color_stop_rgb(0, 0.1, 0.2, 0.3) end)
    assert_error("add_color_stop_rgba on non-gradient pattern",
                 function () pat:add_color_stop_rgba(0, 0.1, 0.2, 0.3, 0.4) end)
end

function test_equality ()
    -- Different userdatas, same Cairo object.
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    local pattern1 = Cairo.pattern_create_rgb(0.25, 0.5, 0.75)
    local cr = Cairo.context_create(surface)
    cr:set_source(pattern1)
    local pattern2 = cr:get_source()
    assert_true(pattern1 == pattern2)

    -- Different userdatas, different Cairo objects.
    local pattern3 = Cairo.pattern_create_rgb(0.25, 0.5, 0.75)
    assert_false(pattern1 == pattern3)
end

function test_double_gc ()
    local surface = Cairo.image_surface_create("rgb24", 23, 45)
    local pattern = Cairo.pattern_create_rgb(0.25, 0.5, 0.75)
    pattern:__gc()
    pattern:__gc()
end

-- vi:ts=4 sw=4 expandtab
