require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.path", lunit.testcase, package.seeall)

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
    local path = cr:copy_path()
    path:__gc()
    path:__gc()
end

function test_current_point ()
    assert_false(cr:has_current_point())
    local x, y = cr:get_current_point()
    assert_nil(x)
    assert_nil(y)

    cr:move_to(1.5, 3.25)
    assert_true(cr:has_current_point())
    x, y = cr:get_current_point()
    assert_equal(1.5, x)
    assert_equal(3.25, y)
end

function test_path_extents ()
    local x1, y1, x2, y2 = cr:path_extents()
    assert_equal(0, x1)
    assert_equal(0, y1)
    assert_equal(0, x2)
    assert_equal(0, y2)

    cr:move_to(1.5, 3.25)
    cr:line_to(4, 7)
    x1, y1, x2, y2 = cr:path_extents()
    assert_equal(1.5, x1)
    assert_equal(3.25, y1)
    assert_equal(4, x2)
    assert_equal(7, y2)
end

function test_clipping ()
    local x1, y1, x2, y2 = cr:clip_extents()

    -- Wrapper functions to make this work with older cairo versions
    local function in_clip(cr, x, y, expected)
        if cr.in_clip then
            return cr:in_clip(3, 4)
        end
        return expected
    end

    assert_equal(0, x1)
    assert_equal(0, y1)
    assert_equal(23, x2)
    assert_equal(45, y2)

    cr:move_to(1.5, 3.25)
    cr:line_to(4, 7)
    cr:line_to(8, 5)
    cr:close_path()
    cr:clip_preserve()
    assert_true(cr:has_current_point())
    assert_true(in_clip(cr, 3, 4, true))
    cr:new_sub_path()
    cr:rectangle(2, 3, 2, 2)
    cr:clip()
    assert_false(cr:has_current_point())
    assert_false(in_clip(cr, 3, 4, false))
    x1, y1, x2, y2 = cr:clip_extents()
    assert_equal(1, x1)
    assert_equal(3, y1)
    assert_equal(8, x2)
    assert_equal(7, y2)

    cr:reset_clip()
    assert_true(in_clip(cr, 3, 4, true))
    x1, y1, x2, y2 = cr:clip_extents()
    assert_equal(0, x1)
    assert_equal(0, y1)
    assert_equal(23, x2)
    assert_equal(45, y2)
end

function test_fill_extents ()
    cr:rectangle(5, 6, 11, 12)
    local x1, y1, x2, y2 = cr:fill_extents()
    assert_equal(5, x1)
    assert_equal(6, y1)
    assert_equal(16, x2)
    assert_equal(18, y2)
end

function test_stroke_extents ()
    cr:move_to(5, 9)
    cr:line_to(17, 9)
    cr:set_line_width(5)
    local x1, y1, x2, y2 = cr:stroke_extents()
    assert_equal(5, x1)
    assert_equal(6.5, y1)
    assert_equal(17, x2)
    assert_equal(11.5, y2)
end

local function check_path_iter (cmd, pt, expcmd, ...)
    assert_string(cmd)
    assert_equal(expcmd, cmd)
    local numexpargs = select("#", ...)
    if numexpargs == 0 then
        assert_nil(pt)
    else
        assert_equal(numexpargs, #pt)
        for i = 1, numexpargs do
            assert_equal(select(i, ...), pt[i], cmd .. " arg " .. i)
        end
    end
end

function test_each ()
    cr:move_to(1, 2)
    cr:line_to(3, 4)
    cr:curve_to(5, 6, 7, 8, 9, 10)
    cr:close_path()
    cr:move_to(11, 12)
    cr:rel_line_to(5, 6)
    cr:rel_move_to(7, 8)
    cr:rel_curve_to(1, 2, 3, 4, 5, 6)

    local path = cr:copy_path()
    assert_userdata(path)
    assert_equal("cairo path object", path._NAME)

    local iter, s, i = path:each()
    local cmd, pt
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "move-to", 1, 2)
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "line-to", 3, 4)
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "curve-to", 5, 6, 7, 8, 9, 10)
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "close-path")
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "move-to", 11, 12)
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "line-to", 16, 18)
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "move-to", 23, 26)
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "curve-to", 24, 28, 26, 30, 28, 32)
    i, cmd, pt = iter(s, i)
    assert_nil(i); assert_nil(cmd); assert_nil(pt)
end

function test_copy_path_flat ()
    cr:move_to(1, 2)
    cr:curve_to(5, 6, 7, 8, 9, 10)

    local path = cr:copy_path_flat()
    assert_userdata(path)
    assert_equal("cairo path object", path._NAME)

    for _, cmd in path:each() do
        assert(cmd ~= "curve-to")
    end
end

function test_append_path ()
    cr:line_to(11, 12)
    cr:line_to(13, 14)
    local path = cr:copy_path()

    cr:new_path()
    cr:move_to(1, 2)
    cr:line_to(3, 4)
    cr:append_path(path)

    path = cr:copy_path()
    local iter, s, i = path:each()
    local cmd, pt
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "move-to", 1, 2)
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "line-to", 3, 4)
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "move-to", 11, 12)
    i, cmd, pt = iter(s, i)
    check_path_iter(cmd, pt, "line-to", 13, 14)
    i = iter(s, i)
    assert_nil(i)
end

-- vi:ts=4 sw=4 expandtab
