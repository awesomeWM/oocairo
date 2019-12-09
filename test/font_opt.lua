require "test-setup"
local lunit = require "lunit"
local Cairo = require "oocairo"

local assert_error      = lunit.assert_error
local assert_true       = lunit.assert_true
local assert_false      = lunit.assert_false
local assert_equal      = lunit.assert_equal
local assert_userdata   = lunit.assert_userdata
local assert_table      = lunit.assert_table
local assert_number     = lunit.assert_number
local assert_match      = lunit.assert_match
local assert_string     = lunit.assert_string
local assert_boolean    = lunit.assert_boolean
local assert_not_equal  = lunit.assert_not_equal

local module = { _NAME="test.font_opt" }

function module.test_create ()
    local opt = Cairo.font_options_create()
    assert_userdata(opt)
    assert_equal("cairo font options object", opt._NAME)
    assert_equal(nil, opt:status())
end

function module.test_double_gc ()
    local opt = Cairo.font_options_create()
    opt:__gc()
    opt:__gc()
end

function module.test_antialias ()
    local opt = Cairo.font_options_create()
    assert_error("bad value", function () opt:set_antialias("foo") end)
    assert_error("missing value", function () opt:set_antialias(nil) end)
    assert_equal("default", opt:get_antialias())
    for _, aa in ipairs{ "default", "none", "gray", "subpixel" } do
        opt:set_antialias(aa)
        assert_equal(aa, opt:get_antialias())
    end

    -- Boolean values also select reasonable values.
    opt:set_antialias(true)
    assert_equal("default", opt:get_antialias())
    opt:set_antialias(false)
    assert_equal("none", opt:get_antialias())
end

function module.test_subpixel_order ()
    local opt = Cairo.font_options_create()
    assert_error("bad value", function () opt:set_subpixel_order("foo") end)
    assert_error("missing value", function () opt:set_subpixel_order(nil) end)
    assert_equal("default", opt:get_subpixel_order())
    for _, aa in ipairs{ "default", "rgb", "bgr", "vrgb", "vbgr" } do
        opt:set_subpixel_order(aa)
        assert_equal(aa, opt:get_subpixel_order())
    end
end

function module.test_hint_style ()
    local opt = Cairo.font_options_create()
    assert_error("bad value", function () opt:set_hint_style("foo") end)
    assert_error("missing value", function () opt:set_hint_style(nil) end)
    assert_equal("default", opt:get_hint_style())
    for _, aa in ipairs{ "default", "none", "slight", "medium", "full" } do
        opt:set_hint_style(aa)
        assert_equal(aa, opt:get_hint_style())
    end
end

function module.test_hint_metrics ()
    local opt = Cairo.font_options_create()
    assert_error("bad value", function () opt:set_hint_metrics("foo") end)
    assert_error("missing value", function () opt:set_hint_metrics(nil) end)
    assert_equal("default", opt:get_hint_metrics())
    for _, aa in ipairs{ "default", "off", "on" } do
        opt:set_hint_metrics(aa)
        assert_equal(aa, opt:get_hint_metrics())
    end
end

function module.test_hash_and_equality ()
    local opt1 = Cairo.font_options_create()
    local opt2 = Cairo.font_options_create()
    assert_number(opt1:hash())
    assert_number(opt2:hash())

    -- Different objects, but they are equal because they represent the
    -- same options, until they are set to hold different values.
    assert_not_equal(tostring(opt1), tostring(opt2))
    assert_equal(opt1:hash(), opt2:hash())
    assert_true(opt1 == opt2)
    opt1:set_antialias("none")
    assert_not_equal(opt1:hash(), opt2:hash())
    assert_false(opt1 == opt2)
end

function module.test_copy ()
    local opt1 = Cairo.font_options_create()
    opt1:set_antialias("gray")
    local opt2 = opt1:copy()
    assert_not_equal(tostring(opt1), tostring(opt2))
    assert_equal(opt1:hash(), opt2:hash())
    assert_true(opt1 == opt2)
    assert_equal("gray", opt1:get_antialias())
    assert_equal("gray", opt2:get_antialias())

    opt1:set_antialias("subpixel")
    assert_not_equal(opt1:hash(), opt2:hash())
    assert_false(opt1 == opt2)
end

function module.test_merge ()
    local opt1 = Cairo.font_options_create()
    local opt2 = Cairo.font_options_create()

    -- Give them a different non-default option each.
    opt1:set_antialias("gray")
    opt2:set_subpixel_order("bgr")

    opt1:merge(opt2)

    -- opt1 should now have both options set.
    assert_equal("gray", opt1:get_antialias())
    assert_equal("bgr", opt1:get_subpixel_order())

    -- opt2 should be unchanged.
    assert_equal("default", opt2:get_antialias())
    assert_equal("bgr", opt2:get_subpixel_order())
end

lunit.testcase(module)
return module

-- vi:ts=4 sw=4 expandtab
