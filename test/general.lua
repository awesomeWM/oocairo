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

local module = { _NAME="test.general" }

function module.test_support_flags ()
    for _, feature in ipairs{
        "HAS_PDF_SURFACE", "HAS_PNG_FUNCTIONS", "HAS_PS_SURFACE",
        "HAS_SVG_SURFACE", "HAS_USER_FONT"
    } do
        assert_boolean(Cairo[feature], "flag for feature " .. feature)
    end
end

lunit.testcase(module)
return module

-- vi:ts=4 sw=4 expandtab
