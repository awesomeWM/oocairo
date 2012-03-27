require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.mesh_pattern", lunit.testcase, package.seeall)

if Cairo.check_version(1, 12, 0) then
    function setup ()
        pattern = Cairo.pattern_create_mesh()
    end

    function teardown ()
        if pattern then
            assert_equal(nil, pattern:status(), "Error status on mesh pattern")
        end
        pattern = nil
    end

    local function add_patches()
        -- Sample taken from cairo's api docs
        pattern:begin_patch()
        pattern:move_to(0, 0)
        pattern:curve_to(30, -30,  60,  30, 100, 0);
        pattern:curve_to(60,  30, 130,  60, 100, 100);
        pattern:curve_to(60,  70,  30, 130,   0, 100);
        pattern:curve_to(30,  70, -30,  30,   0, 0);
        pattern:set_corner_color_rgb(0, 1, 0, 0);
        pattern:set_corner_color_rgb(1, 0, 1, 0);
        pattern:set_corner_color_rgb(2, 0, 0, 1);
        pattern:set_corner_color_rgb(3, 1, 1, 0);
        pattern:end_patch();

        pattern:begin_patch()
        pattern:move_to(100, 100);
        pattern:line_to(130, 130);
        pattern:line_to(130,  70);
        pattern:set_corner_color_rgba(0, 1, 0, 0, 0);
        pattern:set_corner_color_rgba(1, 0, 1, 0, 0.5);
        pattern:set_corner_color_rgba(2, 0, 0, 1, 1);
        pattern:end_patch()
    end

    function test_solid_rgb ()
        local function check(r1, g1, b1, a1, r2, g2, b2, a2)
            assert_equal(r1, r2)
            assert_equal(g1, g2)
            assert_equal(b1, b2)
            assert_equal(a1, a2)
        end

        add_patches()

        check(1, 0, 0, 1, pattern:get_corner_color_rgba(0, 0))
        check(0, 1, 0, 1, pattern:get_corner_color_rgba(0, 1))
        check(0, 0, 1, 1, pattern:get_corner_color_rgba(0, 2))
        check(1, 1, 0, 1, pattern:get_corner_color_rgba(0, 3))

        check(1, 0, 0, 0,   pattern:get_corner_color_rgba(1, 0))
        check(0, 1, 0, 0.5, pattern:get_corner_color_rgba(1, 1))
        check(0, 0, 1, 1,   pattern:get_corner_color_rgba(1, 2))
        check(1, 0, 0, 0,   pattern:get_corner_color_rgba(1, 3))
    end

    function test_patch_count()
        add_patches()
        assert_equal(2, pattern:get_patch_count())
    end

    function test_control_point()
        local points = {
            { 1, 2 },
            { 42, 12 },
            { 23, 21 }
        }

        pattern:begin_patch()
        pattern:move_to(0, 42)
        pattern:move_to(10, 10)
        pattern:set_control_point(0, 1, 1)
        pattern:end_patch()

        pattern:begin_patch()
        pattern:move_to(42, 0)
        pattern:move_to(10, 10)
        for k, v in pairs(points) do
            pattern:set_control_point(k, v[1], v[2])
        end
        pattern:end_patch()

        local function check(x1, y1, x2, y2)
            assert_equal(x1, x2)
            assert_equal(y1, y2)
        end

        check(1,  1, pattern:get_control_point(0, 0))
        check(10, 10, pattern:get_control_point(0, 1))
        check(10, 10, pattern:get_control_point(0, 2))
        check(10, 10, pattern:get_control_point(0, 3))
        for k, v in pairs(points) do
            check(v[1], v[2], pattern:get_control_point(1, k))
        end
    end

    local function check_path_iter (cmd, pt, expcmd, ...)
        if not cmd then print(debug.traceback()) end
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
        pattern:begin_patch()
        pattern:move_to(1, 2)
        pattern:line_to(3, 4)
        pattern:curve_to(5, 6, 7, 8, 9, 10)
        pattern:end_patch()

        pattern:begin_patch()
        pattern:move_to(11, 12)
        pattern:line_to(16, 18)
        pattern:line_to(23, 26)
        pattern:curve_to(24, 28, 26, 30, 28, 32)
        pattern:end_patch()

        local path = pattern:get_path(0)
        assert_userdata(path)
        assert_equal("cairo path object", path._NAME)

        local iter, s, i = path:each()
        local cmd, pt
        i, cmd, pt = iter(s, i)
        check_path_iter(cmd, pt, "move-to", 1, 2)

        local path = pattern:get_path(1)
        assert_userdata(path)
        assert_equal("cairo path object", path._NAME)
    end

    function test_double_gc ()
        pattern:__gc()
        pattern:__gc()
        pattern = nil
    end
end

-- vi:ts=4 sw=4 expandtab
