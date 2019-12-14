/* Copyright (C) 2010-2011 Uli Schlachter
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

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
static int
region_create (lua_State *L) {
    cairo_region_t **reg = create_region_userdata(L);
    *reg = cairo_region_create();
    return 1;
}

static int
region_create_rectangle (lua_State *L) {
    cairo_region_t **reg = create_region_userdata(L);
    cairo_rectangle_int_t rect;

    from_lua_rectangle(L, &rect, 1);
    *reg = cairo_region_create_rectangle(&rect);
    return 1;
}

static int
region_create_rectangles (lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_settop(L, 1); /* discard other args */

    size_t max_entries = lua_objlen (L, 1);
    cairo_rectangle_int_t *rects = lua_newuserdata(L, max_entries * sizeof(cairo_rectangle_int_t));
    size_t i;
    
    /* Now iterate over the table */
    for (i = 0; i < max_entries; ++i) {
        from_lua_rectangle_list_element(L, &rects[i], 1, i+1);
    }

    cairo_region_t **reg = create_region_userdata(L);
    *reg = cairo_region_create_rectangles(rects, i);
    return 1;
}

static int
region_copy (lua_State *L) {
    cairo_region_t **ud = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION);
    cairo_region_t **reg = create_region_userdata(L);
    *reg = cairo_region_copy(*ud);
    return 1;
}

static int
region_eq (lua_State *L) {
    cairo_region_t **obj1 = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION);
    cairo_region_t **obj2 = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_REGION);
    lua_pushboolean(L, cairo_region_equal(*obj1, *obj2));
    return 1;
}

static int
region_gc (lua_State *L) {
    cairo_region_t **ud = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION);
    cairo_region_destroy(*ud);
    *ud = 0;
    return 0;
}

static int
region_contains_point (lua_State *L) {
    cairo_region_t **ud = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    lua_pushboolean(L, cairo_region_contains_point(*ud, x, y));
    return 1;
}

static int
region_contains_rectangle (lua_State *L) {
    cairo_region_t **ud = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION);
    cairo_rectangle_int_t rect;

    from_lua_rectangle(L, &rect, 2);
    return region_overlap_to_lua(L, cairo_region_contains_rectangle(*ud, &rect));
}

static int
region_get_extents (lua_State *L) {
    cairo_region_t **ud = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION);
    cairo_rectangle_int_t rect;

    cairo_region_get_extents(*ud, &rect);
    return to_lua_rectangle(L, &rect);
}

static int
region_get_rectangles (lua_State *L) {
    cairo_region_t **ud = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION);
    int num, i;
    cairo_rectangle_int_t rect;

    lua_newtable(L);
    num = cairo_region_num_rectangles(*ud);
    for (i = 0; i < num; i++) {
        cairo_region_get_rectangle(*ud, i, &rect);
        lua_pushnumber(L, i+1);
        to_lua_rectangle(L, &rect);
        lua_settable(L, -3);
    }
    return 1;
}

static int
region_is_empty (lua_State *L) {
    cairo_region_t **ud = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION);
    cairo_bool_t empty = cairo_region_is_empty(*ud);
    lua_pushboolean(L, empty);
    return 1;
}

static int
region_translate (lua_State *L) {
    cairo_region_t **ud = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    cairo_region_translate(*ud, x, y);
    return 0;
}

static int
region_status (lua_State *L) {
    cairo_region_t **ud = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION);
    return push_cairo_status(L, cairo_region_status(*ud));
}

#define OP(name) \
static int \
region_ ## name (lua_State *L) { \
    cairo_region_t **dest = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION); \
    cairo_region_t **other = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_REGION); \
    return push_cairo_status(L, cairo_region_ ## name (*dest, *other)); \
} \
static int \
region_ ## name ## _rectangle (lua_State *L) { \
    cairo_region_t **dest = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_REGION); \
    cairo_rectangle_int_t rect; \
    from_lua_rectangle(L, &rect, 2); \
    return push_cairo_status(L, cairo_region_ ## name ## _rectangle (*dest, &rect)); \
}

OP(intersect)
OP(subtract)
OP(union)
OP(xor)

#undef OP

static const luaL_Reg
region_methods[] = {
    { "__eq", region_eq },
    { "__gc", region_gc },
    { "contains_point", region_contains_point },
    { "contains_rectangle", region_contains_rectangle },
    { "copy", region_copy },
    { "get_extents", region_get_extents },
    { "get_rectangles", region_get_rectangles },
    { "intersect", region_intersect },
    { "intersect_rectangle", region_intersect_rectangle },
    { "is_empty", region_is_empty },
    { "status", region_status },
    { "subtract", region_subtract },
    { "subtract_rectangle", region_subtract_rectangle },
    { "translate", region_translate },
    { "union", region_union },
    { "union_rectangle", region_union_rectangle },
    { "xor", region_xor },
    { "xor_rectangle", region_xor_rectangle },
    { 0, 0 }
};
#endif

/* vi:set ts=4 sw=4 expandtab: */
