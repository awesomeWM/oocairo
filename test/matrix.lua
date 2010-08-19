require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.matrix", lunit.testcase, package.seeall)

function test_create ()
    local m = Cairo.matrix_create()
    check_matrix_elems(m)
    assert_equal("cairo matrix object", m._NAME)
    assert_equal(1, m[1]); assert_equal(0, m[3]); assert_equal(0, m[5])
    assert_equal(0, m[2]); assert_equal(1, m[4]); assert_equal(0, m[6])
end

function test_transform ()
    local m = Cairo.matrix_create()
    m:translate(3, 4)
    check_matrix_elems(m)
    assert_equal(1, m[1]); assert_equal(0, m[3]); assert_equal(3, m[5])
    assert_equal(0, m[2]); assert_equal(1, m[4]); assert_equal(4, m[6])

    m = Cairo.matrix_create()
    m:scale(2, 3)
    check_matrix_elems(m)
    assert_equal(2, m[1]); assert_equal(0, m[3]); assert_equal(0, m[5])
    assert_equal(0, m[2]); assert_equal(3, m[4]); assert_equal(0, m[6])

    m = Cairo.matrix_create()
    m:rotate(2)
    check_matrix_elems(m)
    assert_not_equal(1, m[1]); assert_not_equal(0, m[3]); assert_equal(0, m[5])
    assert_not_equal(0, m[2]); assert_not_equal(1, m[4]); assert_equal(0, m[6])
end

function test_invert ()
    local m = Cairo.matrix_create()
    m:translate(3, 4)
    m:scale(2, 3)
    m:rotate(2)
    m:invert()
    check_matrix_elems(m)
end

function test_multiply ()
    local m1, m2 = Cairo.matrix_create(), Cairo.matrix_create()
    m1:scale(2, 3)
    m2:translate(4, 5)
    m1:multiply(m2)
    check_matrix_elems(m1)
    assert_equal(2, m1[1]); assert_equal(0, m1[3]); assert_equal(4, m1[5])
    assert_equal(0, m1[2]); assert_equal(3, m1[4]); assert_equal(5, m1[6])
end

function test_apply_transformation ()
    local m = Cairo.matrix_create()
    m:scale(2, 3)
    m:translate(4, 5)

    local x, y = m:transform_point(10, 100)
    assert_equal(28, x)
    assert_equal(315, y)

    x, y = m:transform_distance(10, 100)
    assert_equal(20, x)
    assert_equal(300, y)
end

-- vi:ts=4 sw=4 expandtab
