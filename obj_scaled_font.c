/* Copyright (C) 2007-2008 Geoff Richards
 * Copyright (C) 2010-2011 Uli Schlachter
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

static int
scaled_font_create (lua_State *L) {
    cairo_scaled_font_t **font;
    cairo_matrix_t font_mat, ctm;
    cairo_font_options_t *options = 0;
    int options_needs_freeing = 0;
    cairo_font_face_t **face = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTFACE);
    from_lua_matrix(L, &font_mat, 2);
    from_lua_matrix(L, &ctm, 3);
    if (!lua_isnoneornil(L, 4))
        options = *(cairo_font_options_t **)
                            luaL_checkudata(L, 4, OOCAIRO_MT_NAME_FONTOPT);
    else {
        /* This default font options object is needed because of a bug which
         * prevents Cairo from accepting NULL to indicate default options, as
         * it's documented to do. */
        options = cairo_font_options_create();
        options_needs_freeing = 1;
    }
    font = create_scaledfont_userdata(L);
    assert(*face);
    *font = cairo_scaled_font_create(*face, &font_mat, &ctm, options);
    assert(cairo_scaled_font_status(*font) == CAIRO_STATUS_SUCCESS);
    if (options_needs_freeing)
        cairo_font_options_destroy(options);
    return 1;
}

static int
scaledfont_eq (lua_State *L) {
    cairo_scaled_font_t **obj1 = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_scaled_font_t **obj2 = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_SCALEDFONT);
    lua_pushboolean(L, *obj1 == *obj2);
    return 1;
}

static int
scaledfont_gc (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_scaled_font_destroy(*obj);
    *obj = 0;
    return 0;
}

static int
scaledfont_extents (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_font_extents_t extents;
    cairo_scaled_font_extents(*obj, &extents);
    create_lua_font_extents(L, &extents);
    return 1;
}

static int
scaledfont_get_ctm (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_matrix_t mat;
    cairo_scaled_font_get_ctm(*obj, &mat);
    create_lua_matrix(L, &mat);
    return 1;
}

static int
scaledfont_get_font_face (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_font_face_t **face = create_fontface_userdata(L);
    *face = cairo_scaled_font_get_font_face(*obj);
    cairo_font_face_reference(*face);
    return 1;
}

static int
scaledfont_get_font_matrix (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_matrix_t mat;
    cairo_scaled_font_get_font_matrix(*obj, &mat);
    create_lua_matrix(L, &mat);
    return 1;
}

static int
scaledfont_get_font_options (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_font_options_t **opt = create_fontopt_userdata(L);
    *opt = cairo_font_options_create();
    cairo_scaled_font_get_font_options(*obj, *opt);
    return 1;
}

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int
scaledfont_get_scale_matrix (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_matrix_t mat;
    cairo_scaled_font_get_scale_matrix(*obj, &mat);
    create_lua_matrix(L, &mat);
    return 1;
}
#endif

static int
scaledfont_get_type (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    return font_type_to_lua(L, cairo_scaled_font_get_type(*obj));
}

static int
scaledfont_glyph_extents (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_glyph_t *glyphs;
    int num_glyphs;
    cairo_text_extents_t extents;
    from_lua_glyph_array(L, &glyphs, &num_glyphs, 2);
    cairo_scaled_font_glyph_extents(*obj, glyphs, num_glyphs, &extents);
    if (glyphs)
        GLYPHS_FREE(glyphs);
    create_lua_text_extents(L, &extents);
    return 1;
}

static int
scaledfont_text_extents (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_text_extents_t extents;
    cairo_scaled_font_text_extents(*obj, luaL_checkstring(L, 2), &extents);
    create_lua_text_extents(L, &extents);
    return 1;
}

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int
scaledfont_text_to_glyphs (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    double x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    size_t text_len;
    const char *text = luaL_checklstring(L, 4, &text_len);
    cairo_glyph_t *glyphs = 0;
    cairo_text_cluster_t *clusters = 0;
    cairo_text_cluster_flags_t cluster_flags = 0;
    int num_glyphs = 0, num_clusters = 0;

    if (cairo_scaled_font_text_to_glyphs(
            *obj, x, y, text, text_len,
            &glyphs, &num_glyphs,
            &clusters, &num_clusters, &cluster_flags) != CAIRO_STATUS_SUCCESS)
        return luaL_error(L, "error converting text to glyphs");

    create_lua_glyph_array(L, glyphs, num_glyphs);
    GLYPHS_FREE(glyphs);
    create_lua_text_cluster_table(L, clusters, num_clusters, cluster_flags);
    cairo_text_cluster_free(clusters);
    return 2;
}
#endif

static int
scaledfont_status (lua_State *L) {
    cairo_scaled_font_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SCALEDFONT);
    return push_cairo_status(L, cairo_scaled_font_status(*obj));
}

static const luaL_Reg
scaledfont_methods[] = {
    { "__eq", scaledfont_eq },
    { "__gc", scaledfont_gc },
    { "extents", scaledfont_extents },
    { "get_ctm", scaledfont_get_ctm },
    { "get_font_face", scaledfont_get_font_face },
    { "get_font_matrix", scaledfont_get_font_matrix },
    { "get_font_options", scaledfont_get_font_options },
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    { "get_scale_matrix", scaledfont_get_scale_matrix },
#endif
    { "get_type", scaledfont_get_type },
    { "glyph_extents", scaledfont_glyph_extents },
    { "text_extents", scaledfont_text_extents },
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    { "text_to_glyphs", scaledfont_text_to_glyphs },
#endif
    { "status", scaledfont_status },
    { 0, 0 }
};

/* vi:set ts=4 sw=4 expandtab: */
