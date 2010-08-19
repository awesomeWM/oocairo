-- Show the effect of all the different the 'operator' types on compositing
-- two image surfaces with rectangles drawn on.
--
-- This example is based on a C program from a Cairo tutorial here:
--     http://zetcode.com/tutorials/cairographicstutorial/compositing/

local Cairo = require "oocairo"

-- The operators to test, arranged as they are in the output image.
local OPERATORS = {
    "add", "atop", "clear", "dest", "dest-atop", "dest-in", "dest-out",
    "dest-over", "in", "out", "over", "saturate", "source", "xor",
}

local MARGIN = 15
local OP_WD, OP_HT = 60, 90     -- height includes space for text
local OPS_PER_ROW = 7
local NUM_ROWS = #OPERATORS / OPS_PER_ROW
local IMG_WD = OPS_PER_ROW * OP_WD + (OPS_PER_ROW + 1) * MARGIN
local IMG_HT = NUM_ROWS * OP_HT + (NUM_ROWS + 1) * MARGIN

local function draw (cr, x, y, w, h, op)
    local first = Cairo.surface_create_similar(cr:get_target(), "color-alpha",
                                               w, h)
    local second = Cairo.surface_create_similar(cr:get_target(), "color-alpha",
                                                w, h)

    local first_cr = Cairo.context_create(first)
    first_cr:set_source_rgb(0, 0, 0.4)
    first_cr:rectangle(x, y, 50, 50)
    first_cr:fill()

    local second_cr = Cairo.context_create(second)
    second_cr:set_source_rgb(0.5, 0.5, 0)
    second_cr:rectangle(x + 10, y + 20, 50, 50)
    second_cr:fill()

    first_cr:set_operator(op)
    first_cr:set_source(second, 0, 0)
    first_cr:paint()

    cr:set_source(first, 0, 0)
    cr:paint()

    -- Put a label underneath each example with the name of the operator.
    cr:set_source_rgb(0, 0, 0)
    local label_width = cr:text_extents(op).width
    cr:move_to(x + (OP_WD - label_width) / 2, y + OP_HT - 3)    -- centered
    cr:show_text(op)
end

local surface = Cairo.image_surface_create("rgb24", IMG_WD, IMG_HT)
local cr = Cairo.context_create(surface)

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

cr:select_font_face("Bitstream Vera Sans")
cr:set_font_size(12)

local x, y = MARGIN, MARGIN
for i, op in ipairs(OPERATORS) do
    draw(cr, x, y, IMG_WD, IMG_HT, op)
    x = x + OP_WD + MARGIN
    if i % OPS_PER_ROW == 0 then x = MARGIN; y = y + OP_HT + MARGIN end
end

surface:write_to_png("compositing.png")

-- vi:ts=4 sw=4 expandtab
