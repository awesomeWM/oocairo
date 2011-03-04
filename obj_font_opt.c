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
font_options_create (lua_State *L) {
    cairo_font_options_t **opt = create_fontopt_userdata(L);
    *opt = cairo_font_options_create();
    return 1;
}

static int
fontopt_eq (lua_State *L) {
    cairo_font_options_t **obj1 = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    cairo_font_options_t **obj2 = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_FONTOPT);
    lua_pushboolean(L, cairo_font_options_equal(*obj1, *obj2));
    return 1;
}

static int
fontopt_gc (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    cairo_font_options_destroy(*obj);
    *obj = 0;
    return 0;
}

static int
fontopt_copy (lua_State *L) {
    cairo_font_options_t **orig = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    cairo_font_options_t **newobj = create_fontopt_userdata(L);
    *newobj = cairo_font_options_copy(*orig);
    return 1;
}

static int
fontopt_get_antialias (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    return antialias_to_lua(L, cairo_font_options_get_antialias(*obj));
}

static int
fontopt_get_hint_metrics (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    return hint_metrics_to_lua(L, cairo_font_options_get_hint_metrics(*obj));
}

static int
fontopt_get_hint_style (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    return hint_style_to_lua(L, cairo_font_options_get_hint_style(*obj));
}

static int
fontopt_get_subpixel_order (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    return subpixel_order_to_lua(L,
                        cairo_font_options_get_subpixel_order(*obj));
}

static int
fontopt_hash (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    lua_pushnumber(L, cairo_font_options_hash(*obj));
    return 1;
}

static int
fontopt_merge (lua_State *L) {
    cairo_font_options_t **obj1 = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    cairo_font_options_t **obj2 = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_FONTOPT);
    cairo_font_options_merge(*obj1, *obj2);
    return 0;
}

static int
fontopt_set_antialias (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    cairo_font_options_set_antialias(*obj, antialias_from_lua(L, 2));
    return 0;
}

static int
fontopt_set_hint_metrics (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    cairo_font_options_set_hint_metrics(*obj, hint_metrics_from_lua(L, 2));
    return 0;
}

static int
fontopt_set_hint_style (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    cairo_font_options_set_hint_style(*obj, hint_style_from_lua(L, 2));
    return 0;
}

static int
fontopt_set_subpixel_order (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    cairo_font_options_set_subpixel_order(*obj, subpixel_order_from_lua(L, 2));
    return 0;
}

static int
fontopt_status (lua_State *L) {
    cairo_font_options_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_FONTOPT);
    return push_cairo_status(L, cairo_font_options_status(*obj));
}

static const luaL_Reg
fontopt_methods[] = {
    { "__eq", fontopt_eq },
    { "__gc", fontopt_gc },
    { "copy", fontopt_copy },
    { "get_antialias", fontopt_get_antialias },
    { "get_hint_metrics", fontopt_get_hint_metrics },
    { "get_hint_style", fontopt_get_hint_style },
    { "get_subpixel_order", fontopt_get_subpixel_order },
    { "hash", fontopt_hash },
    { "merge", fontopt_merge },
    { "set_antialias", fontopt_set_antialias },
    { "set_hint_metrics", fontopt_set_hint_metrics },
    { "set_hint_style", fontopt_set_hint_style },
    { "set_subpixel_order", fontopt_set_subpixel_order },
    { "status", fontopt_status },
    { 0, 0 }
};

/* vi:set ts=4 sw=4 expandtab: */
