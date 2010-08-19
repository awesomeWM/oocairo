require "test-setup"
require "lunit"
local Cairo = require "oocairo"

module("test.general", lunit.testcase, package.seeall)

function test_support_flags ()
    for _, feature in ipairs{
        "HAS_PDF_SURFACE", "HAS_PNG_FUNCTIONS", "HAS_PS_SURFACE",
        "HAS_SVG_SURFACE", "HAS_USER_FONT"
    } do
        assert_boolean(Cairo[feature], "flag for feature " .. feature)
    end
end

-- vi:ts=4 sw=4 expandtab
