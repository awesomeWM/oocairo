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
pattern_create_rgb (lua_State *L) {
    cairo_pattern_t **obj = create_pattern_userdata(L);
    double r = luaL_checknumber(L, 1), g = luaL_checknumber(L, 2),
           b = luaL_checknumber(L, 3);
    *obj = cairo_pattern_create_rgb(r, g, b);
    return 1;
}

static int
pattern_create_rgba (lua_State *L) {
    cairo_pattern_t **obj = create_pattern_userdata(L);
    double r = luaL_checknumber(L, 1), g = luaL_checknumber(L, 2),
           b = luaL_checknumber(L, 3), a = luaL_checknumber(L, 4);
    *obj = cairo_pattern_create_rgba(r, g, b, a);
    return 1;
}

static int
pattern_create_for_surface (lua_State *L) {
    cairo_surface_t **surface = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_pattern_t **obj = create_pattern_userdata(L);
    *obj = cairo_pattern_create_for_surface(*surface);
    return 1;
}

static int
pattern_create_linear (lua_State *L) {
    cairo_pattern_t **obj = create_pattern_userdata(L);
    double x0 = luaL_checknumber(L, 1), y0 = luaL_checknumber(L, 2),
           x1 = luaL_checknumber(L, 3), y1 = luaL_checknumber(L, 4);
    *obj = cairo_pattern_create_linear(x0, y0, x1, y1);
    return 1;
}

static int
pattern_create_radial (lua_State *L) {
    cairo_pattern_t **obj = create_pattern_userdata(L);
    double cx0 = luaL_checknumber(L, 1), cy0 = luaL_checknumber(L, 2),
           radius0 = luaL_checknumber(L, 3),
           cx1 = luaL_checknumber(L, 4), cy1 = luaL_checknumber(L, 5),
           radius1 = luaL_checknumber(L, 6);
    *obj = cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1);
    return 1;
}

static int
pattern_eq (lua_State *L) {
    cairo_pattern_t **obj1 = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    cairo_pattern_t **obj2 = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_PATTERN);
    lua_pushboolean(L, *obj1 == *obj2);
    return 1;
}

static int
pattern_gc (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    cairo_pattern_destroy(*obj);
    *obj = 0;
    return 0;
}

static int
pattern_add_color_stop_rgb (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    cairo_pattern_type_t type = cairo_pattern_get_type(*obj);
    if (type != CAIRO_PATTERN_TYPE_LINEAR && type != CAIRO_PATTERN_TYPE_RADIAL)
        return luaL_error(L, "add_color_stop_rgb() only works on gradient"
                          " patterns");
    cairo_pattern_add_color_stop_rgb(*obj, luaL_checknumber(L, 2),
        luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5));
    return 0;
}

static int
pattern_add_color_stop_rgba (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    cairo_pattern_type_t type = cairo_pattern_get_type(*obj);
    if (type != CAIRO_PATTERN_TYPE_LINEAR && type != CAIRO_PATTERN_TYPE_RADIAL)
        return luaL_error(L, "add_color_stop_rgba() only works on gradient"
                          " patterns");
    cairo_pattern_add_color_stop_rgba(*obj, luaL_checknumber(L, 2),
        luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5),
        luaL_checknumber(L, 6));
    return 0;
}

static int
pattern_get_color_stops (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    int count, i;
    double offset, r, g, b, a;
    if (cairo_pattern_get_color_stop_count(*obj, &count)
            == CAIRO_STATUS_PATTERN_TYPE_MISMATCH)
        luaL_error(L, "pattern is not a gradient");
    lua_createtable(L, count, 0);
    for (i = 0; i < count; ++i) {
        cairo_pattern_get_color_stop_rgba(*obj, i, &offset, &r, &g, &b, &a);
        lua_createtable(L, 5, 0);
        lua_pushnumber(L, offset);
        lua_rawseti(L, -2, 1);
        lua_pushnumber(L, r);
        lua_rawseti(L, -2, 2);
        lua_pushnumber(L, g);
        lua_rawseti(L, -2, 3);
        lua_pushnumber(L, b);
        lua_rawseti(L, -2, 4);
        lua_pushnumber(L, a);
        lua_rawseti(L, -2, 5);
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

static int
pattern_get_extend (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    return extend_to_lua(L, cairo_pattern_get_extend(*obj));
}

static int
pattern_get_filter (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    return filter_to_lua(L, cairo_pattern_get_filter(*obj));
}

static int
pattern_get_linear_points (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    double x0, y0, x1, y1;
    if (cairo_pattern_get_linear_points(*obj, &x0, &y0, &x1, &y1)
            == CAIRO_STATUS_PATTERN_TYPE_MISMATCH)
        luaL_error(L, "pattern is not a linear gradient");
    lua_pushnumber(L, x0);
    lua_pushnumber(L, y0);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    return 4;
}

static int
pattern_get_matrix (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    cairo_matrix_t mat;
    cairo_pattern_get_matrix(*obj, &mat);
    create_lua_matrix(L, &mat);
    return 1;
}

static int
pattern_get_radial_circles (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    double x0, y0, r0, x1, y1, r1;
    if (cairo_pattern_get_radial_circles(*obj, &x0, &y0, &r0, &x1, &y1, &r1)
            == CAIRO_STATUS_PATTERN_TYPE_MISMATCH)
        luaL_error(L, "pattern is not a radial gradient");
    lua_pushnumber(L, x0);
    lua_pushnumber(L, y0);
    lua_pushnumber(L, r0);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, r1);
    return 6;
}

static int
pattern_get_rgba (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    double r, g, b, a;
    if (cairo_pattern_get_rgba(*obj, &r, &g, &b, &a)
            == CAIRO_STATUS_PATTERN_TYPE_MISMATCH)
        luaL_error(L, "pattern is not a solid color");
    lua_pushnumber(L, r);
    lua_pushnumber(L, g);
    lua_pushnumber(L, b);
    lua_pushnumber(L, a);
    return 4;
}

static int
pattern_get_surface (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    cairo_surface_t *surface;
    SurfaceUserdata *surfobj;
    if (cairo_pattern_get_surface(*obj, &surface)
            == CAIRO_STATUS_PATTERN_TYPE_MISMATCH)
        luaL_error(L, "pattern is not a surface pattern");
    surfobj = create_surface_userdata(L);
    cairo_surface_reference(surface);
    surfobj->surface = surface;
    return 1;
}

static int
pattern_get_type (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    switch (cairo_pattern_get_type(*obj)) {
        case CAIRO_PATTERN_TYPE_SOLID:   lua_pushliteral(L, "solid");    break;
        case CAIRO_PATTERN_TYPE_SURFACE: lua_pushliteral(L, "surface");  break;
        case CAIRO_PATTERN_TYPE_LINEAR:  lua_pushliteral(L, "linear");   break;
        case CAIRO_PATTERN_TYPE_RADIAL:  lua_pushliteral(L, "radial");   break;
        default:                         lua_pushliteral(L, "<invalid>");
    }
    return 1;
}

static int
pattern_set_extend (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    cairo_pattern_set_extend(*obj, extend_from_lua(L, 2));
    return 0;
}

static int
pattern_set_filter (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    cairo_pattern_set_filter(*obj, filter_from_lua(L, 2));
    return 0;
}

static int
pattern_set_matrix (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    cairo_matrix_t mat;
    from_lua_matrix(L, &mat, 2);
    cairo_pattern_set_matrix(*obj, &mat);
    return 0;
}

static int
pattern_status (lua_State *L) {
    cairo_pattern_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATTERN);
    return push_cairo_status(L, cairo_pattern_status(*obj));
}

static const luaL_Reg
pattern_methods[] = {
    { "__eq", pattern_eq },
    { "__gc", pattern_gc },
    { "add_color_stop_rgb", pattern_add_color_stop_rgb },
    { "add_color_stop_rgba", pattern_add_color_stop_rgba },
    { "get_color_stops", pattern_get_color_stops },
    { "get_extend", pattern_get_extend },
    { "get_filter", pattern_get_filter },
    { "get_linear_points", pattern_get_linear_points },
    { "get_matrix", pattern_get_matrix },
    { "get_radial_circles", pattern_get_radial_circles },
    { "get_rgba", pattern_get_rgba },
    { "get_surface", pattern_get_surface },
    { "get_type", pattern_get_type },
    { "set_extend", pattern_set_extend },
    { "set_filter", pattern_set_filter },
    { "set_matrix", pattern_set_matrix },
    { "status", pattern_status },
    { 0, 0 }
};

/* vi:set ts=4 sw=4 expandtab: */
