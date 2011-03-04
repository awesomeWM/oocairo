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

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int
toy_font_face_create (lua_State *L) {
    cairo_font_face_t **face;
    cairo_font_slant_t slant = CAIRO_FONT_SLANT_NORMAL;
    cairo_font_weight_t weight = CAIRO_FONT_WEIGHT_NORMAL;
    if (!lua_isnoneornil(L, 2))
        slant = font_slant_from_lua(L, 2);
    if (!lua_isnoneornil(L, 3))
        weight = font_weight_from_lua(L, 3);
    face = create_fontface_userdata(L);
    *face = cairo_toy_font_face_create(luaL_checkstring(L, 1), slant, weight);
    return 1;
}
#endif

#ifdef CAIRO_HAS_USER_FONT
typedef struct UserFontInfo_ {
    lua_State *L;
    int ref;
} UserFontInfo;
static cairo_user_data_key_t user_font_udata_key;
#define USERFONT_CALLBACK_INIT 1
#define USERFONT_CALLBACK_RENDER_GLYPH 2
#define USERFONT_CALLBACK_TEXT_TO_GLYPHS 3
#define USERFONT_CALLBACK_UNICODE_TO_GLYPH 4

static void
user_font_udata_free (void *udata) {
    UserFontInfo *info = udata;
    luaL_unref(info->L, LUA_REGISTRYINDEX, info->ref);
    free(info);
}

static cairo_status_t
user_font_init (cairo_scaled_font_t *font, cairo_t *cr,
                cairo_font_extents_t *extents)
{
    cairo_t **crp;
    cairo_scaled_font_t **fontp;
    UserFontInfo *info = cairo_font_face_get_user_data(
            cairo_scaled_font_get_font_face(font), &user_font_udata_key);
    lua_rawgeti(info->L, LUA_REGISTRYINDEX, info->ref);
    create_lua_font_extents(info->L, extents);
    lua_rawgeti(info->L, -2, USERFONT_CALLBACK_INIT);
    fontp = create_scaledfont_userdata(info->L);
    *fontp = font;
    cairo_scaled_font_reference(font);
    crp = create_context_userdata(info->L);
    *crp = cr;
    cairo_reference(cr);
    lua_pushvalue(info->L, -4);
    lua_call(info->L, 3, 0);
    from_lua_font_extents(info->L, extents);
    lua_pop(info->L, 2);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
user_font_render_glyph (cairo_scaled_font_t *font, unsigned long glyph,
                        cairo_t *cr, cairo_text_extents_t *extents)
{
    cairo_t **crp;
    cairo_scaled_font_t **fontp;
    UserFontInfo *info = cairo_font_face_get_user_data(
            cairo_scaled_font_get_font_face(font), &user_font_udata_key);
    lua_rawgeti(info->L, LUA_REGISTRYINDEX, info->ref);
    create_lua_text_extents(info->L, extents);
    lua_rawgeti(info->L, -2, USERFONT_CALLBACK_RENDER_GLYPH);
    fontp = create_scaledfont_userdata(info->L);
    *fontp = font;
    cairo_scaled_font_reference(font);
    lua_pushnumber(info->L, glyph);
    crp = create_context_userdata(info->L);
    *crp = cr;
    cairo_reference(cr);
    lua_pushvalue(info->L, -5);
    lua_call(info->L, 4, 0);
    from_lua_text_extents(info->L, extents);
    lua_pop(info->L, 2);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
user_font_text_to_glyphs (cairo_scaled_font_t *font, const char *utf8,
                          int utf8_len, cairo_glyph_t **glyphs, int *num_glyphs,
                          cairo_text_cluster_t **clusters, int *num_clusters,
                          cairo_text_cluster_flags_t *cluster_flags)
{
    cairo_scaled_font_t **fontp;
    UserFontInfo *info = cairo_font_face_get_user_data(
            cairo_scaled_font_get_font_face(font), &user_font_udata_key);
    lua_rawgeti(info->L, LUA_REGISTRYINDEX, info->ref);
    lua_rawgeti(info->L, -1, USERFONT_CALLBACK_TEXT_TO_GLYPHS);
    fontp = create_scaledfont_userdata(info->L);
    *fontp = font;
    cairo_scaled_font_reference(font);
    lua_pushlstring(info->L, utf8, utf8_len);
    lua_pushboolean(info->L, !!clusters);   /* true if cluster info is wanted */
    lua_call(info->L, 3, 2);

    if (lua_isnil(info->L, -2))
        *num_glyphs = -1;
    else {
        *glyphs = 0;
        from_lua_glyph_array(info->L, glyphs, num_glyphs,
                             lua_gettop(info->L) - 1);
    }

    if (clusters && !lua_isnil(info->L, -1)) {
        *clusters = 0;
        from_lua_clusters_table(info->L, clusters, num_clusters, cluster_flags,
                                lua_gettop(info->L));
    }

    lua_pop(info->L, 3);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
user_font_unicode_to_glyph (cairo_scaled_font_t *font, unsigned long unicode,
                            unsigned long *glyph_index)
{
    cairo_scaled_font_t **fontp;
    UserFontInfo *info = cairo_font_face_get_user_data(
            cairo_scaled_font_get_font_face(font), &user_font_udata_key);
    lua_rawgeti(info->L, LUA_REGISTRYINDEX, info->ref);
    lua_rawgeti(info->L, -1, USERFONT_CALLBACK_UNICODE_TO_GLYPH);
    fontp = create_scaledfont_userdata(info->L);
    *fontp = font;
    cairo_scaled_font_reference(font);
    lua_pushnumber(info->L, unicode);
    lua_call(info->L, 2, 1);
    if (!lua_isnumber(info->L, -1))
        luaL_error(info->L, "bad glyph index returned from 'unicode_to_glyph'"
                   " callback, should be number");
    *glyph_index = lua_tonumber(info->L, -1);
    lua_pop(info->L, 1);
    return CAIRO_STATUS_SUCCESS;
}

typedef cairo_status_t (*UserFontCallbackFunc) (void);
typedef void (*UserFontCallbackSetFunc) (cairo_font_face_t *font,
                                         UserFontCallbackFunc func);
typedef struct UserFontCallback_ {
    const char *name;
    UserFontCallbackFunc call_func;
    UserFontCallbackSetFunc set_func;
    int index;
} UserFontCallback;
static const UserFontCallback user_font_callbacks[] = {
    { "init", (UserFontCallbackFunc) user_font_init,
      (UserFontCallbackSetFunc) cairo_user_font_face_set_init_func,
      USERFONT_CALLBACK_INIT },
    { "render_glyph", (UserFontCallbackFunc) user_font_render_glyph,
      (UserFontCallbackSetFunc) cairo_user_font_face_set_render_glyph_func,
      USERFONT_CALLBACK_RENDER_GLYPH },
    { "text_to_glyphs", (UserFontCallbackFunc) user_font_text_to_glyphs,
      (UserFontCallbackSetFunc) cairo_user_font_face_set_text_to_glyphs_func,
      USERFONT_CALLBACK_TEXT_TO_GLYPHS },
    { "unicode_to_glyph", (UserFontCallbackFunc) user_font_unicode_to_glyph,
      (UserFontCallbackSetFunc) cairo_user_font_face_set_unicode_to_glyph_func,
      USERFONT_CALLBACK_UNICODE_TO_GLYPH },
    { 0, 0, 0, 0 }
};

static int
user_font_face_create (lua_State *L) {
    cairo_font_face_t **face = create_fontface_userdata(L);
    UserFontInfo *info;
    const UserFontCallback *callback;

    luaL_checktype(L, 1, LUA_TTABLE);
    *face = cairo_user_font_face_create();

    lua_createtable(L, 4, 0);
    info = malloc(sizeof(UserFontInfo));
    assert(info);
    info->L = L;
    info->ref = LUA_NOREF;

    for (callback = user_font_callbacks; callback->name; ++callback) {
        lua_getfield(L, 1, callback->name);
        if (lua_isnil(L, -1)) {
            if (callback->index == USERFONT_CALLBACK_RENDER_GLYPH) {
                user_font_udata_free(info);
                return luaL_error(L, "the '%s' option is required",
                                  callback->name);
            }
            lua_pop(L, 1);
        }
        else if (!lua_isfunction(L, -1)) {
            user_font_udata_free(info);
            return luaL_error(L, "the '%s' option must be a function",
                              callback->name);
        }
        else {
            lua_rawseti(L, -2, callback->index);
            callback->set_func(*face, callback->call_func);
        }
    }

    info->ref = luaL_ref(L, LUA_REGISTRYINDEX);

    cairo_font_face_set_user_data(*face, &user_font_udata_key, info,
                                  user_font_udata_free);

    return 1;
}
#endif

static int
fontface_eq (lua_State *L) {
    cairo_font_face_t **obj1 = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTFACE);
    cairo_font_face_t **obj2 = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_FONTFACE);
    lua_pushboolean(L, *obj1 == *obj2);
    return 1;
}

static int
fontface_gc (lua_State *L) {
    cairo_font_face_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTFACE);
    cairo_font_face_destroy(*obj);
    *obj = 0;
    return 0;
}

static int
fontface_get_type (lua_State *L) {
    cairo_font_face_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTFACE);
    return font_type_to_lua(L, cairo_font_face_get_type(*obj));
}

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int
fontface_get_family (lua_State *L) {
    cairo_font_face_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTFACE);
    if (cairo_font_face_get_type(*obj) != CAIRO_FONT_TYPE_TOY)
        return luaL_error(L, "'get_family' only works on toy font faces");
    lua_pushstring(L, cairo_toy_font_face_get_family(*obj));
    return 1;
}

static int
fontface_get_slant (lua_State *L) {
    cairo_font_face_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTFACE);
    if (cairo_font_face_get_type(*obj) != CAIRO_FONT_TYPE_TOY)
        return luaL_error(L, "'get_slant' only works on toy font faces");
    return font_slant_to_lua(L, cairo_toy_font_face_get_slant(*obj));
}

static int
fontface_get_weight (lua_State *L) {
    cairo_font_face_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTFACE);
    if (cairo_font_face_get_type(*obj) != CAIRO_FONT_TYPE_TOY)
        return luaL_error(L, "'get_weight' only works on toy font faces");
    return font_weight_to_lua(L, cairo_toy_font_face_get_weight(*obj));
}
#endif

static int
fontface_status (lua_State *L) {
    cairo_font_face_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTFACE);
    return push_cairo_status(L, cairo_font_face_status(*obj));
}

static const luaL_Reg
fontface_methods[] = {
    { "__eq", fontface_eq },
    { "__gc", fontface_gc },
    { "get_type", fontface_get_type },
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    { "get_family", fontface_get_family },
    { "get_slant", fontface_get_slant },
    { "get_weight", fontface_get_weight },
#endif
    { "status", fontface_status },
    { 0, 0 }
};

/* vi:set ts=4 sw=4 expandtab: */
