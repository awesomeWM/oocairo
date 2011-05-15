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
context_create (lua_State *L) {
    cairo_surface_t **surface = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_t **obj = create_context_userdata(L);
    *obj = cairo_create(*surface);
    return 1;
}

/* Call the Lua function gtk.gdk_cairo_create() to create a context object.
 * I'm not calling it from C, because that would mean the library would
 * have to link with Gtk+ at compile time.  Much more flexible to go through
 * the Lua 'gtk' binding and just convert the return value into our own kind
 * of object. */
static int
context_create_gdk (lua_State *L) {
    luaL_argcheck(L, !lua_isnoneornil(L, 1), 1, "expected GdkDrawable object");
    get_gtk_module_function(L, "gdk_cairo_create");
    lua_pushvalue(L, 1);
    lua_call(L, 1, 1);
    if (!lua_isuserdata(L, 1))
        return luaL_error(L, "return value from gtk.gdk_cairo_create() is not"
                          " a userdata");
    /* Switch to using my metatable for the object.  This of course relies
     * on Lua-Gnome using the same format for storing the pointer. */
    luaL_getmetatable(L, OOCAIRO_MT_NAME_CONTEXT);
    lua_setmetatable(L, -2);
    return 1;
}

static int
cr_gc (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_destroy(*obj);
    *obj = 0;
    return 0;
}

static int
cr_append_path (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_path_t **path = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_PATH);
    cairo_append_path(*obj, *path);
    return 0;
}

static int
cr_arc (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_arc(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3),
              luaL_checknumber(L, 4), luaL_checknumber(L, 5),
              luaL_checknumber(L, 6));
    return 0;
}

static int
cr_arc_negative (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_arc_negative(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3),
                       luaL_checknumber(L, 4), luaL_checknumber(L, 5),
                       luaL_checknumber(L, 6));
    return 0;
}

static int
cr_clip (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_clip(*obj);
    return 0;
}

static int
cr_clip_extents (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double x1, y1, x2, y2;
    cairo_clip_extents(*obj, &x1, &y1, &x2, &y2);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, x2);
    lua_pushnumber(L, y2);
    return 4;
}

static int
cr_clip_preserve (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_clip_preserve(*obj);
    return 0;
}

static int
cr_close_path (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_close_path(*obj);
    return 0;
}

static int
cr_copy_path (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_path_t **path = lua_newuserdata(L, sizeof(cairo_path_t *));
    *path = 0;
    luaL_getmetatable(L, OOCAIRO_MT_NAME_PATH);
    lua_setmetatable(L, -2);
    *path = cairo_copy_path(*obj);
    return 1;
}

static int
cr_copy_path_flat (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_path_t **path = lua_newuserdata(L, sizeof(cairo_path_t *));
    *path = 0;
    luaL_getmetatable(L, OOCAIRO_MT_NAME_PATH);
    lua_setmetatable(L, -2);
    *path = cairo_copy_path_flat(*obj);
    return 1;
}

static int
cr_curve_to (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_curve_to(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3),
                   luaL_checknumber(L, 4), luaL_checknumber(L, 5),
                   luaL_checknumber(L, 6), luaL_checknumber(L, 7));
    return 0;
}

static int
cr_device_to_user (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    cairo_device_to_user(*obj, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

static int
cr_device_to_user_distance (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    cairo_device_to_user_distance(*obj, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

static int
cr_fill (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_fill(*obj);
    return 0;
}

static int
cr_fill_extents (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double x1, y1, x2, y2;
    cairo_fill_extents(*obj, &x1, &y1, &x2, &y2);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, x2);
    lua_pushnumber(L, y2);
    return 4;
}

static int
cr_fill_preserve (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_fill_preserve(*obj);
    return 0;
}

static int
cr_font_extents (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_font_extents_t extents;
    cairo_font_extents(*obj, &extents);
    create_lua_font_extents(L, &extents);
    return 1;
}

static int
cr_get_antialias (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    return antialias_to_lua(L, cairo_get_antialias(*obj));
}

static int
cr_get_current_point (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double x, y;
    if (!cairo_has_current_point(*obj))
        return 0;
    cairo_get_current_point(*obj, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

static int
cr_get_dash (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    int cnt, i;
    double *dashes = 0, offset;

    cnt = cairo_get_dash_count(*obj);
    if (cnt > 0) {
        dashes = malloc(sizeof(double) * cnt);
        assert(dashes);
    }

    cairo_get_dash(*obj, dashes, &offset);

    lua_createtable(L, cnt, 0);
    for (i = 0; i < cnt; ++i) {
        lua_pushnumber(L, dashes[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_pushnumber(L, offset);

    if (cnt > 0)
        free(dashes);
    return 2;
}

static int
cr_get_fill_rule (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    return fill_rule_to_lua(L, cairo_get_fill_rule(*obj));
}

static int
cr_get_font_face (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_font_face_t **face = create_fontface_userdata(L);
    *face = cairo_get_font_face(*obj);
    cairo_font_face_reference(*face);
    return 1;
}

static int
cr_get_font_matrix (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_matrix_t mat;
    cairo_get_font_matrix(*obj, &mat);
    create_lua_matrix(L, &mat);
    return 1;
}

static int
cr_get_font_options (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_font_options_t **opt = create_fontopt_userdata(L);
    *opt = cairo_font_options_create();
    cairo_get_font_options(*obj, *opt);
    return 1;
}

static int
cr_get_group_target (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    SurfaceUserdata *surface = create_surface_userdata(L);
    surface->surface = cairo_get_group_target(*obj);
    cairo_surface_reference(surface->surface);
    return 1;
}

static int
cr_get_line_cap (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    return line_cap_to_lua(L, cairo_get_line_cap(*obj));
}

static int
cr_get_line_join (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    return line_join_to_lua(L, cairo_get_line_join(*obj));
}

static int
cr_get_line_width (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    lua_pushnumber(L, cairo_get_line_width(*obj));
    return 1;
}

static int
cr_get_matrix (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_matrix_t mat;
    cairo_get_matrix(*obj, &mat);
    create_lua_matrix(L, &mat);
    return 1;
}

static int
cr_get_miter_limit (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    lua_pushnumber(L, cairo_get_miter_limit(*obj));
    return 1;
}

static int
cr_get_operator (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    return operator_to_lua(L, cairo_get_operator(*obj));
}

static int
cr_get_scaled_font (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_scaled_font_t **font = create_scaledfont_userdata(L);
    *font = cairo_get_scaled_font(*obj);
    cairo_scaled_font_reference(*font);
    return 1;
}

static int
cr_get_source (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_pattern_t **pattern = create_pattern_userdata(L);
    *pattern = cairo_get_source(*obj);
    cairo_pattern_reference(*pattern);
    return 1;
}

static int
cr_get_target (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    SurfaceUserdata *surface = create_surface_userdata(L);
    surface->surface = cairo_get_target(*obj);
    cairo_surface_reference(surface->surface);
    return 1;
}

static int
cr_get_tolerance (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    lua_pushnumber(L, cairo_get_tolerance(*obj));
    return 1;
}

static int
cr_glyph_extents (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_glyph_t *glyphs;
    int num_glyphs;
    cairo_text_extents_t extents;
    from_lua_glyph_array(L, &glyphs, &num_glyphs, 2);
    cairo_glyph_extents(*obj, glyphs, num_glyphs, &extents);
    if (glyphs)
        GLYPHS_FREE(glyphs);
    create_lua_text_extents(L, &extents);
    return 1;
}

static int
cr_glyph_path (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_glyph_t *glyphs;
    int num_glyphs;
    from_lua_glyph_array(L, &glyphs, &num_glyphs, 2);
    cairo_glyph_path(*obj, glyphs, num_glyphs);
    if (glyphs)
        GLYPHS_FREE(glyphs);
    return 1;
}

static int
cr_has_current_point (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    lua_pushboolean(L, cairo_has_current_point(*obj));
    return 1;
}

static int
cr_identity_matrix (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_identity_matrix(*obj);
    return 0;
}

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
static int
cr_in_clip (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    lua_pushboolean(L,
        cairo_in_clip(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
    return 1;
}
#endif

static int
cr_in_fill (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    lua_pushboolean(L,
        cairo_in_fill(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
    return 1;
}

static int
cr_in_stroke (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    lua_pushboolean(L,
        cairo_in_stroke(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
    return 1;
}

static int
cr_line_to (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_line_to(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
    return 0;
}

static int
cr_move_to (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_move_to(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
    return 0;
}

static int
cr_mask (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    void *p;
    cairo_pattern_t **pattern;
    cairo_surface_t **surface;

    /* This is needed to make sure the stack positions for the optional
     * arguments stay nil after we start pushing other things on. */
    lua_settop(L, 4);

    if ((p = lua_touserdata(L, 2))) {
        if (lua_getmetatable(L, 2)) {
            lua_getfield(L, LUA_REGISTRYINDEX, OOCAIRO_MT_NAME_PATTERN);
            if (lua_rawequal(L, -1, -2)) {
                pattern = p;
                cairo_mask(*obj, *pattern);
                return 0;
            }
            lua_pop(L, 1);

            lua_getfield(L, LUA_REGISTRYINDEX, OOCAIRO_MT_NAME_SURFACE);
            if (lua_rawequal(L, -1, -2)) {
                surface = p;
                cairo_mask_surface(*obj, *surface,
                                   luaL_optnumber(L, 3, 0),
                                   luaL_optnumber(L, 4, 0));
                return 0;
            }
            lua_pop(L, 2);
        }
    }

    return luaL_typerror(L, 2, "Cairo pattern or surface object");
}

static int
cr_new_path (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_new_path(*obj);
    return 0;
}

static int
cr_new_sub_path (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_new_sub_path(*obj);
    return 0;
}

static int
cr_paint (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_paint(*obj);
    return 0;
}

static int
cr_paint_with_alpha (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_paint_with_alpha(*obj, luaL_checknumber(L, 2));
    return 0;
}

static int
cr_path_extents (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double x1, y1, x2, y2;
    cairo_path_extents(*obj, &x1, &y1, &x2, &y2);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, x2);
    lua_pushnumber(L, y2);
    return 4;
}

static int
cr_pop_group (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_pattern_t **pattern = create_pattern_userdata(L);
    *pattern = cairo_pop_group(*obj);
    return 1;
}

static int
cr_pop_group_to_source (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_pop_group_to_source(*obj);
    return 0;
}

static int
cr_push_group (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_content_t content = CAIRO_CONTENT_COLOR_ALPHA;
    if (!lua_isnoneornil(L, 2))
        content = content_from_lua(L, 2);
    cairo_push_group_with_content(*obj, content);
    return 0;
}

static int
cr_rectangle (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_rectangle(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3),
                    luaL_checknumber(L, 4), luaL_checknumber(L, 5));
    return 0;
}

static int
cr_rel_curve_to (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_rel_curve_to(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3),
                       luaL_checknumber(L, 4), luaL_checknumber(L, 5),
                       luaL_checknumber(L, 6), luaL_checknumber(L, 7));
    return 0;
}

static int
cr_rel_line_to (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_rel_line_to(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
    return 0;
}

static int
cr_rel_move_to (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_rel_move_to(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
    return 0;
}

static int
cr_reset_clip (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_reset_clip(*obj);
    return 0;
}

static int
cr_restore (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_restore(*obj);
    return 0;
}

static int
cr_rotate (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_rotate(*obj, luaL_checknumber(L, 2));
    return 0;
}

static int
cr_save (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_save(*obj);
    return 0;
}

static int
cr_scale (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_scale(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
    return 0;
}

static int
cr_select_font_face (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_font_slant_t slant = CAIRO_FONT_SLANT_NORMAL;
    cairo_font_weight_t weight = CAIRO_FONT_WEIGHT_NORMAL;
    if (!lua_isnoneornil(L, 3))
        slant = font_slant_from_lua(L, 3);
    if (!lua_isnoneornil(L, 4))
        weight = font_weight_from_lua(L, 4);
    cairo_select_font_face(*obj, luaL_checkstring(L, 2), slant, weight);
    return 0;
}

static int
cr_set_antialias (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_set_antialias(*obj, antialias_from_lua(L, 2));
    return 0;
}

static int
cr_set_dash (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    int num_dashes, i;
    double *dashes = 0, offset, n, dashtotal;

    luaL_checktype(L, 2, LUA_TTABLE);
    offset = luaL_checknumber(L, 3);

    num_dashes = lua_objlen(L, 2);
    if (num_dashes > 0) {
        dashes = malloc(sizeof(double) * num_dashes);
        assert(dashes);
        dashtotal = 0;

        for (i = 0; i < num_dashes; ++i) {
            lua_rawgeti(L, 2, i + 1);
            if (!lua_isnumber(L, -1)) {
                free(dashes);
                return luaL_error(L, "bad dash pattern, dash value %d isn't"
                                  " a number", i + 1);
            }
            n = lua_tonumber(L, -1);
            if (n < 0) {
                free(dashes);
                return luaL_error(L, "bad dash pattern, dash value %d is"
                                  " negative", i + 1);
            }
            dashes[i] = n;
            dashtotal += n;
            lua_pop(L, 1);
        }

        if (dashtotal == 0) {
            free(dashes);
            return luaL_error(L, "bad dash pattern, all values are zero");
        }
    }

    cairo_set_dash(*obj, dashes, num_dashes, offset);

    if (num_dashes > 0)
        free(dashes);
    return 0;
}

static int
cr_set_fill_rule (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_set_fill_rule(*obj, fill_rule_from_lua(L, 2));
    return 0;
}

static int
cr_set_font_face (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_font_face_t *face = 0;
    if (!lua_isnoneornil(L, 2))
        face = *(cairo_font_face_t **) luaL_checkudata(L, 2, OOCAIRO_MT_NAME_FONTFACE);
    cairo_set_font_face(*obj, face);
    return 0;
}

static int
cr_set_font_matrix (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_matrix_t mat;
    from_lua_matrix(L, &mat, 2);
    cairo_set_font_matrix(*obj, &mat);
    return 0;
}

static int
cr_set_font_options (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_font_options_t **opt = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_FONTOPT);
    cairo_set_font_options(*obj, *opt);
    return 0;
}

static int
cr_set_font_size (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_set_font_size(*obj, luaL_checknumber(L, 2));
    return 0;
}

static int
cr_set_line_cap (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_set_line_cap(*obj, line_cap_from_lua(L, 2));
    return 0;
}

static int
cr_set_line_join (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_set_line_join(*obj, line_join_from_lua(L, 2));
    return 0;
}

static int
cr_set_line_width (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double n = luaL_checknumber(L, 2);
    luaL_argcheck(L, n >= 0, 2, "line width cannot be negative");
    cairo_set_line_width(*obj, n);
    return 0;
}

static int
cr_set_matrix (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_matrix_t mat;
    from_lua_matrix(L, &mat, 2);
    cairo_set_matrix(*obj, &mat);
    return 0;
}

static int
cr_set_miter_limit (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_set_miter_limit(*obj, luaL_checknumber(L, 2));
    return 0;
}

static int
cr_set_operator (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_set_operator(*obj, operator_from_lua(L, 2));
    return 0;
}

static int
cr_set_scaled_font (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_scaled_font_t **font = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_SCALEDFONT);
    cairo_set_scaled_font(*obj, *font);
    return 0;
}

static int
cr_set_source (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    void *p;
    cairo_pattern_t **pattern;
    cairo_surface_t **surface;

    /* This is needed to make sure the stack positions for the optional
     * arguments stay nil after we start pushing other things on. */
    lua_settop(L, 4);

    if ((p = lua_touserdata(L, 2))) {
        if (lua_getmetatable(L, 2)) {
            lua_getfield(L, LUA_REGISTRYINDEX, OOCAIRO_MT_NAME_PATTERN);
            if (lua_rawequal(L, -1, -2)) {
                pattern = p;
                cairo_set_source(*obj, *pattern);
                return 0;
            }
            lua_pop(L, 1);

            lua_getfield(L, LUA_REGISTRYINDEX, OOCAIRO_MT_NAME_SURFACE);
            if (lua_rawequal(L, -1, -2)) {
                surface = p;
                cairo_set_source_surface(*obj, *surface,
                                         luaL_optnumber(L, 3, 0),
                                         luaL_optnumber(L, 4, 0));
                return 0;
            }
            lua_pop(L, 2);
        }
    }

    return luaL_typerror(L, 2, "Cairo pattern or surface object");
}

static double
get_color_component_from_lua (lua_State *L, int pos, const char *field) {
    double color;
    lua_getfield(L, pos, field);
    if (!lua_isnumber(L, -1))
        luaL_error(L, "couldn't retrieve colour '%s' from GdkColor", field);
    color = lua_tonumber(L, -1) / 0xFFFF;
    lua_pop(L, 1);
    return color;
}

/* This doesn't bind the native C function 'gdk_cairo_set_source_color',
 * because the GdkColor is a Lua value from the Lua-Gnome library, so we
 * just pull out the three colour components as Lua code would.  Also,
 * we accept an optional alpha value in Gdk format, because otherwise
 * you wouldn't be able to use this for setting a colour from a
 * GtkColorButton for example. */
static int
cr_set_source_gdk_color (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double red   = get_color_component_from_lua(L, 2, "red");
    double green = get_color_component_from_lua(L, 2, "green");
    double blue  = get_color_component_from_lua(L, 2, "blue");
    if (lua_isnoneornil(L, 3))
        cairo_set_source_rgb(*obj, red, green, blue);
    else {
        cairo_set_source_rgba(*obj, red, green, blue,
                              luaL_checknumber(L, 3) / 0xFFFF);
    }
    return 0;
}

/* Use a function provided in the 'gtk' library (which must already be loaded)
 * to set the source to a GdkPixbuf or GdkPixmap (next function).  We trick
 * that library into thinking that our Cairo userdata is really one of its
 * own objects, to avoid calling the C functions directly and therefore
 * having another compile-time dependency. */
static int
cr_set_source_pixbuf (lua_State *L) {
    luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    luaL_argcheck(L, !lua_isnoneornil(L, 2), 2, "expected GdkPixbuf object");
    luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected number for x");
    luaL_argcheck(L, lua_isnumber(L, 4), 4, "expected number for y");
    get_gtk_module_function(L, "gdk_cairo_set_source_pixbuf");
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_pushvalue(L, 4);
    lua_call(L, 4, 0);
    return 0;
}

static int
cr_set_source_pixmap (lua_State *L) {
    luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    luaL_argcheck(L, !lua_isnoneornil(L, 2), 2, "expected GdkPixmap object");
    luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected number for x");
    luaL_argcheck(L, lua_isnumber(L, 4), 4, "expected number for y");
    get_gtk_module_function(L, "gdk_cairo_set_source_pixmap");
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_pushvalue(L, 4);
    lua_call(L, 4, 0);
    return 0;
}

static int
cr_set_source_rgb (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_set_source_rgb(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3),
                         luaL_checknumber(L, 4));
    return 0;
}

static int
cr_set_source_rgba (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_set_source_rgba(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3),
                          luaL_checknumber(L, 4), luaL_checknumber(L, 5));
    return 0;
}

static int
cr_set_tolerance (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_set_tolerance(*obj, luaL_checknumber(L, 2));
    return 0;
}

static int
cr_show_glyphs (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_glyph_t *glyphs;
    int num_glyphs;
    from_lua_glyph_array(L, &glyphs, &num_glyphs, 2);
    cairo_show_glyphs(*obj, glyphs, num_glyphs);
    if (glyphs)
        GLYPHS_FREE(glyphs);
    return 0;
}

static int
cr_show_text (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_show_text(*obj, luaL_checkstring(L, 2));
    return 0;
}

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int
cr_show_text_glyphs (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    size_t text_len;
    const char *text = luaL_checklstring(L, 2, &text_len);
    cairo_glyph_t *glyphs;
    cairo_text_cluster_t *clusters;
    cairo_text_cluster_flags_t cluster_flags;
    int num_glyphs, num_clusters;

    from_lua_glyph_array(L, &glyphs, &num_glyphs, 3);
    from_lua_clusters_table(L, &clusters, &num_clusters, &cluster_flags, 4);

    cairo_show_text_glyphs(*obj, text, text_len, glyphs, num_glyphs,
                           clusters, num_clusters, cluster_flags);
    if (glyphs)
        GLYPHS_FREE(glyphs);
    if (clusters)
        cairo_text_cluster_free(clusters);
    return 0;
}
#endif

static int
cr_stroke (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_stroke(*obj);
    return 0;
}

static int
cr_stroke_extents (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double x1, y1, x2, y2;
    cairo_stroke_extents(*obj, &x1, &y1, &x2, &y2);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, x2);
    lua_pushnumber(L, y2);
    return 4;
}

static int
cr_stroke_preserve (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_stroke_preserve(*obj);
    return 0;
}

static int
cr_text_extents (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_text_extents_t extents;
    cairo_text_extents(*obj, luaL_checkstring(L, 2), &extents);
    create_lua_text_extents(L, &extents);
    return 1;
}

static int
cr_text_path (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_text_path(*obj, luaL_checkstring(L, 2));
    return 0;
}

static int
cr_transform (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_matrix_t mat;
    from_lua_matrix(L, &mat, 2);
    cairo_transform(*obj, &mat);
    return 0;
}

static int
cr_translate (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    cairo_translate(*obj, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
    return 0;
}

static int
cr_user_to_device (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    cairo_user_to_device(*obj, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

static int
cr_user_to_device_distance (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    double x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    cairo_user_to_device_distance(*obj, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

static int
cr_status (lua_State *L) {
    cairo_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_CONTEXT);
    return push_cairo_status(L, cairo_status(*obj));
}

static const luaL_Reg
context_methods[] = {
    { "__gc", cr_gc },
    { "append_path", cr_append_path },
    { "arc", cr_arc },
    { "arc_negative", cr_arc_negative },
    { "clip", cr_clip },
    { "clip_extents", cr_clip_extents },
    { "clip_preserve", cr_clip_preserve },
    { "close_path", cr_close_path },
    { "copy_path", cr_copy_path },
    { "copy_path_flat", cr_copy_path_flat },
    { "curve_to", cr_curve_to },
    { "device_to_user", cr_device_to_user },
    { "device_to_user_distance", cr_device_to_user_distance },
    { "fill", cr_fill },
    { "fill_extents", cr_fill_extents },
    { "fill_preserve", cr_fill_preserve },
    { "font_extents", cr_font_extents },
    { "get_antialias", cr_get_antialias },
    { "get_current_point", cr_get_current_point },
    { "get_dash", cr_get_dash },
    { "get_fill_rule", cr_get_fill_rule },
    { "get_font_face", cr_get_font_face },
    { "get_font_matrix", cr_get_font_matrix },
    { "get_font_options", cr_get_font_options },
    { "get_group_target", cr_get_group_target },
    { "get_line_cap", cr_get_line_cap },
    { "get_line_join", cr_get_line_join },
    { "get_line_width", cr_get_line_width },
    { "get_matrix", cr_get_matrix },
    { "get_miter_limit", cr_get_miter_limit },
    { "get_operator", cr_get_operator },
    { "get_scaled_font", cr_get_scaled_font },
    { "get_source", cr_get_source },
    { "get_target", cr_get_target },
    { "get_tolerance", cr_get_tolerance },
    { "glyph_extents", cr_glyph_extents },
    { "glyph_path", cr_glyph_path },
    { "has_current_point", cr_has_current_point },
    { "identity_matrix", cr_identity_matrix },
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
    { "in_clip", cr_in_clip },
#endif
    { "in_fill", cr_in_fill },
    { "in_stroke", cr_in_stroke },
    { "line_to", cr_line_to },
    { "move_to", cr_move_to },
    { "mask", cr_mask },
    { "new_path", cr_new_path },
    { "new_sub_path", cr_new_sub_path },
    { "paint", cr_paint },
    { "paint_with_alpha", cr_paint_with_alpha },
    { "path_extents", cr_path_extents },
    { "pop_group", cr_pop_group },
    { "pop_group_to_source", cr_pop_group_to_source },
    { "push_group", cr_push_group },
    { "rectangle", cr_rectangle },
    { "rel_curve_to", cr_rel_curve_to },
    { "rel_line_to", cr_rel_line_to },
    { "rel_move_to", cr_rel_move_to },
    { "reset_clip", cr_reset_clip },
    { "restore", cr_restore },
    { "rotate", cr_rotate },
    { "save", cr_save },
    { "scale", cr_scale },
    { "select_font_face", cr_select_font_face },
    { "set_antialias", cr_set_antialias },
    { "set_dash", cr_set_dash },
    { "set_fill_rule", cr_set_fill_rule },
    { "set_font_face", cr_set_font_face },
    { "set_font_matrix", cr_set_font_matrix },
    { "set_font_options", cr_set_font_options },
    { "set_font_size", cr_set_font_size },
    { "set_line_cap", cr_set_line_cap },
    { "set_line_join", cr_set_line_join },
    { "set_line_width", cr_set_line_width },
    { "set_matrix", cr_set_matrix },
    { "set_miter_limit", cr_set_miter_limit },
    { "set_operator", cr_set_operator },
    { "set_scaled_font", cr_set_scaled_font },
    { "set_source", cr_set_source },
    { "set_source_gdk_color", cr_set_source_gdk_color },
    { "set_source_pixbuf", cr_set_source_pixbuf },
    { "set_source_pixmap", cr_set_source_pixmap },
    { "set_source_rgb", cr_set_source_rgb },
    { "set_source_rgba", cr_set_source_rgba },
    { "set_tolerance", cr_set_tolerance },
    { "show_glyphs", cr_show_glyphs },
    { "show_text", cr_show_text },
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    { "show_text_glyphs", cr_show_text_glyphs },
#endif
    { "status", cr_status },
    { "stroke", cr_stroke },
    { "stroke_extents", cr_stroke_extents },
    { "stroke_preserve", cr_stroke_preserve },
    { "text_extents", cr_text_extents },
    { "text_path", cr_text_path },
    { "transform", cr_transform },
    { "translate", cr_translate },
    { "user_to_device", cr_user_to_device },
    { "user_to_device_distance", cr_user_to_device_distance },
    { 0, 0 }
};

/* vi:set ts=4 sw=4 expandtab: */
