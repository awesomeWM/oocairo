-- This is another example of what can be done with user fonts, but this
-- time with glyph images and extents taken from real fonts but rendered
-- with distortion.
--
-- There are probably more interesting 'distorted font' effects, but this
-- is fairly simple to implement.  The normal glyph shape is simply cut
-- by some randomly placed thin lines passing through it at different
-- angles.

local Cairo = require "oocairo"

if not Cairo.user_font_face_create then
    error("User font example requires module built with Cairo 1.8", 0)
end

local FONT_SIZE = 70
local PI = math.asin(1) * 2

local function make_chopped_font (basefont)
    local function render_glyph (font, glyph_index, cr, extents)
        -- Use a temporary context to select the base font and copy the
        -- appropriate glyph extents into the 'extents' argument.  That is,
        -- the user font glyph extents should be the same as those of
        -- the base font.
        local glyphcr = Cairo.context_create(cr:get_target())
        glyphcr:set_scaled_font(basefont)
        cr:scale(1/FONT_SIZE, 1/FONT_SIZE)
        local glyph_tbl = { { glyph_index, 0, 0 } }
        local glyph_extents = glyphcr:glyph_extents(glyph_tbl)
        for k, v in pairs(glyph_extents) do extents[k] = v / FONT_SIZE end

        -- Construct a temporary image surface to draw the glyph on before
        -- it's copied to the 'cr' we were provided with by Cairo.  This
        -- could probably be avoided somehow, and certainly isn't ideal
        -- from an efficiency point of view.  Note that we're using the 'a8'
        -- format, since the glyph image is greyscale, so we don't need colour.
        local wd, ht = cr:user_to_device_distance(glyph_extents.width + 1,
                                                  glyph_extents.height + 1)
        local tmpsurface = Cairo.image_surface_create("a8", wd, ht)
        local tmpcr = Cairo.context_create(tmpsurface)

        -- Fill in the glyph shape by copying its path from the context with
        -- the base font selected and drawing it onto the temporary surface.
        glyphcr:glyph_path(glyph_tbl)
        tmpcr:new_path()
        tmpcr:translate(-glyph_extents.x_bearing, -glyph_extents.y_bearing)
        tmpcr:append_path(glyphcr:copy_path())
        tmpcr:set_source_rgba(0, 0, 0, 1)
        tmpcr:fill()

        -- Construct a path for a bunch of random lines going across the
        -- glyph shape.
        tmpcr:identity_matrix()
        local cx, cy = wd / 2, ht / 2
        local radius = cx * cx + cy * cy
        for _ = 1, 15 do
            local ang = math.random() * 2 * PI
            local xd, yd = radius * math.cos(ang), radius * math.sin(ang)
            local xoffset = math.random() * cx - cx / 2
            local yoffset = math.random() * cy - cy / 2
            tmpcr:move_to(cx + -xd + xoffset, cy + -yd + yoffset)
            tmpcr:line_to(cx + xd + xoffset,  cy + yd + yoffset)
        end

        -- Select a 'hair line' width, that is a line which is one pixel
        -- wide in the output, whatever the font scaling is.  Actually, this
        -- will just use the horizontal distance covered by a pixel, so
        -- hopefully the pixels are close to being square.
        tmpcr:set_line_width(cr:device_to_user_distance(1, 1))

        -- Remove the ink from the glyph shape wherever the lines pass, leaving
        -- transparent gaps.
        tmpcr:set_source_rgba(0, 0, 0, .8)
        tmpcr:set_operator("dest-out")  -- remove ink rather than adding it
        tmpcr:stroke()

        -- Use the distorted glyph shape we've constructed to ink in the
        -- appropriate parts of the output, on the context which Cairo is
        -- using to create the glyph shape to cache.
        cr:mask(tmpsurface, glyph_extents.x_bearing, glyph_extents.y_bearing)
    end

    local function text_to_glyphs (font, text, want_clusters)
        -- This just passes the request on to the base font, although we
        -- have to scale the positions back down to font units, because
        -- they'll be scaled up to the font size of the base font.
        local g = basefont:text_to_glyphs(0, 0, text)
        for i, t in ipairs(g) do
            t[2] = t[2] / FONT_SIZE
            t[3] = t[3] / FONT_SIZE
        end
        return g
    end

    return Cairo.user_font_face_create({
        render_glyph = render_glyph,
        text_to_glyphs = text_to_glyphs,
    })
end

local surface = Cairo.image_surface_create("rgb24", 525, 300)
local cr = Cairo.context_create(surface)

-- White background.
cr:set_source_rgb(1, 1, 1)
cr:paint()

local x, y = 15, 5 + FONT_SIZE

-- Draw lines of text, each in a different colour and font.
for _, info in ipairs{
    { "Quick brown", { .3, 0, 0 },
      { "sans", "normal", "bold" } },
    { "fox jumped…", { 0, .3, 0 },
      { "serif", "italic" } },
    -- The last row won't show up unless you have a font with cuneiform
    -- characters in.  They're just random characters, not meaningful text,
    -- but a bit more interesting to look at than boring old Latin letters.
    { "𒀽𒍬𒍅𒌤", { 0, 0, .3 },
      { "akkadian" } },
} do
    -- Create a normal Cairo 'toy' font to use as the base.  The glyphs from
    -- this font will be used with our custom rendering to distort them.
    local basefont_face = Cairo.toy_font_face_create(unpack(info[3]))
    local font_mat = Cairo.matrix_create()
    font_mat:scale(FONT_SIZE, FONT_SIZE)
    local ctm = Cairo.matrix_create()
    local basefont = Cairo.scaled_font_create(basefont_face, font_mat, ctm)

    cr:set_font_face(make_chopped_font(basefont))
    cr:set_font_size(FONT_SIZE)

    local line, color = unpack(info)
    cr:move_to(x, y)
    cr:set_source_rgb(unpack(color))
    cr:show_text(line)

    y = y + basefont:extents().height * 1.2
end

surface:write_to_png("text-userfont2.png")

-- vi:ts=4 sw=4 expandtab
