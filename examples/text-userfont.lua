-- A simple demonstration of creating a 'user font' in Lua.  The new user
-- font feature in Cairo 1.8 allows you to define a font's metrics and
-- glyph images by providing callback functions, which Lua calls as needed.
-- The idea is that you can provide Cairo with access to fonts in unusual
-- formats it wouldn't normally understand, or create special effects.
--
-- Note that for now at least user fonts can't contain colour information,
-- only shapes drawn with the Cairo context provided to the rendering
-- callback.  You're not allowed to use the 'set_source' methods on it.
-- Also, there seem to be some problems with generating PDF output with
-- user fonts.
--
-- This example has callbacks which make a font based purely on the Unicode
-- codepoint numbers used for characters.  Each character is drawn as a box
-- with some diamonds in, which indicate which bits are set in the codepoint
-- number.  I can't think of any practical use for this, but I wanted
-- something simple to draw which wouldn't require a bunch of ancillary
-- font data.
--
-- I'm relying on the default glyph index mapping, which is that the glyph
-- index numbers are exactly the same as the Unicode codepoints of the
-- characters.  You can do more interesting things by providing other
-- callback functions, but for this example we just need the rendering one.
-- The rendering callback also provides the metrics for each character.
-- We actually leave them as their defaults, except for the width of the
-- glyphs, which can be wider for larger numbered characters (because we
-- display them with several columns of bits).

local Cairo = require "oocairo"

if not Cairo.user_font_face_create then
    error("User font example requires module built with Cairo 1.8", 0)
end

local BIT_WIDTH, BIT_HEIGHT, CHAR_GAP = 0.2, 1 / 8, 0.1

local function make_bit_font ()
    local function render_glyph (font, glyph, cr, extents)
        if glyph == 32 then return end      -- space, just default advance

        -- Draw a diamond shape for each bit which is set within the codepoint
        -- number, with eight bits per column.
        local n = glyph
        local x = 0
        while n ~= 0 do
            local byte = n % 0x100
            n = (n - byte) / 0x100
            for i = 0, 7 do
                local bit = byte % 2
                byte = (byte - bit) / 2
                if bit ~= 0 then
                    cr:move_to(x, (i + 0.5) * BIT_HEIGHT)
                    cr:rel_line_to(BIT_WIDTH / 2, BIT_HEIGHT / 2)
                    cr:rel_line_to(BIT_WIDTH / 2, -BIT_HEIGHT / 2)
                    cr:rel_line_to(-BIT_WIDTH / 2, -BIT_HEIGHT / 2)
                    cr:close_path()
                    cr:fill()
                end
            end
            x = x + BIT_WIDTH
        end

        -- Box round edge, 1 pixel wide and aligned on a pixel in device
        -- coordinates (so that it's sharp, and as thin as possible).
        local xpix, ypix = cr:device_to_user_distance(1, 1)
        cr:rectangle(xpix / 2, ypix / 2, x - xpix, 1 - ypix)
        cr:set_line_width(xpix)
        cr:stroke()
        extents.x_advance = x + CHAR_GAP
    end

    return Cairo.user_font_face_create({ render_glyph = render_glyph })
end

local surface = Cairo.image_surface_create("rgb24", 440, 230)
local cr = Cairo.context_create(surface)

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

cr:set_font_face(make_bit_font())
cr:set_font_size(50)

-- Draw lines of text, each in a different colour.
local x, y = 20, 20
for _, info in ipairs{
    { "The quick brown fox,", { .5, 0, 0 } },
    { "jumped lazy dog…", { 0, .5, 0 } },
    -- These characters will have three columns of bits because they have
    -- large Unicode codepoint numbers.  They're actually a random selection
    -- of cuneiform characters.
    { "𒀽𒍬𒍭𒍅𒌡𒌤", { 0, 0, .5 } },
} do
    local line, color = unpack(info)
    cr:move_to(x, y)
    cr:set_source_rgb(unpack(color))
    cr:show_text(line)
    y = y + 70
end

surface:write_to_png("text-userfont.png")

-- vi:ts=4 sw=4 expandtab
