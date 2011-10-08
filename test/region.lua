require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.region", lunit.testcase, package.seeall)

if Cairo.check_version(1, 10, 0) then
    local function check_region (surface, desc)
        assert_userdata(surface, desc .. ", userdata")
        assert_equal("cairo region object", surface._NAME, desc .. ", mt name")
    end

    function test_double_gc ()
        local region = Cairo.region_create()
        region:__gc()
        region:__gc()
    end

    function test_equal ()
        local reg1 = Cairo.region_create()
        local reg2 = Cairo.region_create_rectangle({ x = 0, y = 0, width = 2, height = 2 })
        assert_equal(reg1, reg1)
        assert_not_equal(reg1, reg2);
        assert_equal(reg2, reg2)
    end

    function test_contains_point ()
        local reg = Cairo.region_create()
        assert_false(reg:contains_point(2, 2))
        reg:union_rectangle({ x = 0, y = 0, width = 4, height = 4 })
        assert_true(reg:contains_point(2, 2))
    end

    function test_contains_rectangle ()
        local reg = Cairo.region_create()
        local rect = { x = 0, y = 0, width = 4, height = 4 }
        assert_equal(reg:contains_rectangle(rect), "overlap-out")
        reg:union_rectangle(rect)
        assert_equal(reg:contains_rectangle(rect), "overlap-in")
        rect.width = 8
        assert_equal(reg:contains_rectangle(rect), "overlap-part")
    end

    function test_copy ()
        local reg1 = Cairo.region_create_rectangle({ x = 0, y = 0, width = 2, height = 2 })
        local reg2 = reg1:copy()
        assert_equal(reg1, reg2)
        reg1:translate(1, 1)
        assert_not_equal(reg1, reg2)
    end

    function test_extents ()
        local rect = { x = 0, y = 1, width = 2, height = 3 }
        local reg = Cairo.region_create_rectangle(rect)
        local extents = reg:get_extents()
        assert_equal(rect[x], extents[x])
        assert_equal(rect[y], extents[y])
        assert_equal(rect[width], extents[width])
        assert_equal(rect[height], extents[height])
    end

    function test_get_rectangles ()
        local rect = { x = 0, y = 0, width = 2, height = 3 }
        local reg = Cairo.region_create_rectangle(rect)
        local saw1, saw2 = false, false
        reg:translate(10, 10)
        reg:union_rectangle(rect)
        for _, v in pairs(reg:get_rectangles()) do
            assert_equal(v.width, 2)
            assert_equal(v.height, 3)
            if v.x == 0 then
                saw1 = true
            elseif v.x == 10 then
                saw2 = true
            end
        end
        assert_true(saw1 and saw2)
    end

    function test_is_empty ()
        local reg = Cairo.region_create()
        assert_true(reg:is_empty())
        reg:union_rectangle({ x = 0, y = 0, width = 1, height = 1 })
        assert_false(reg:is_empty())
    end

    for _, v in pairs({ "intersect", "subtract", "union", "xor" }) do
        _M["test_" .. v] = function()
            local reg1 = Cairo.region_create()
            local reg2 = Cairo.region_create()
            reg1[v](reg1, reg2)
        end
        _M["test_" .. v .. "_rectangle"] = function()
            local reg = Cairo.region_create()
            local rect = { x = 0, y = 0, width = 10, height = 10 }
            reg[v .. "_rectangle"](reg, rect)
        end
    end

    function test_region_create ()
        local rect1 = { x = 0, y = 0, width = 1, height = 1 }
        local rect2 = { x = 10, y = 10, width = 1, height = 1 }
        local rects = { rect1, rect2 }

        local reg = Cairo.region_create()

        local reg = Cairo.region_create_rectangle(rect1)
        local rect = reg:get_rectangles()
        assert_equal(#rect, 1)
        assert_equal(rect[1].x, 0)
        assert_equal(rect[1].y, 0)
        assert_equal(rect[1].width, 1)
        assert_equal(rect[1].height, 1)

        local reg = Cairo.region_create_rectangles(rects)
        local rect = reg:get_rectangles()
        assert_equal(#rect, 2)
        assert_equal(rect[1].x, 0)
        assert_equal(rect[1].y, 0)
        assert_equal(rect[1].width, 1)
        assert_equal(rect[1].height, 1)
        assert_equal(rect[2].x, 10)
        assert_equal(rect[2].y, 10)
        assert_equal(rect[2].width, 1)
        assert_equal(rect[2].height, 1)
    end
end

-- vi:ts=4 sw=4 expandtab
