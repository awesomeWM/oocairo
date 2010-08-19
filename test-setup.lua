-- This will load the new copy of the library on Unix systems where it's built
-- with libtool.
package.cpath = ".libs/liblua-?.so;" .. package.cpath

local _tmp_filenames = {}
function tmpname ()
    local filename = os.tmpname()
    _tmp_filenames[#_tmp_filenames + 1] = filename
    return filename
end

function clean_up_temp_files ()
    for _, filename in ipairs(_tmp_filenames) do
        assert(os.remove(filename))
    end
    _tmp_filenames = {}
end

-- Arbitrary drawing just to make sure there's something in the output file.
function draw_arbitrary_stuff (Cairo, surface)
    local cr = Cairo.context_create(surface)
    cr:move_to(50, 50)
    cr:line_to(250, 150)
    cr:set_line_width(5)
    cr:set_source_rgb(1, 0, 0.5)
    cr:stroke()
end

function check_hash_of_numbers (got, expected)
    lunit.assert_table(got)

    -- Check that all expected keys are present with right type of value.
    local expected_keys = {}
    for _, k in ipairs(expected) do
        lunit.assert_number(got[k])
        expected_keys[k] = true
    end

    -- Check for any unexpected keys.
    for k in pairs(got) do
        lunit.assert_true(expected_keys[k])
    end
end

function check_text_extents (extents)
    check_hash_of_numbers(extents, {
        "x_bearing", "y_bearing", "width", "height", "x_advance", "y_advance",
    })
end

function check_font_extents (extents)
    check_hash_of_numbers(extents, {
        "ascent", "descent", "height", "max_x_advance", "max_y_advance",
    })
end

function check_matrix_elems (m)
    lunit.assert_table(m)
    for k, v in pairs(m) do
        lunit.assert_number(k)
        lunit.assert_number(v)
        lunit.assert(k >= 1 and k <= 6, "bad value for key: " .. tostring(k))
    end
end

-- vi:ts=4 sw=4 expandtab
