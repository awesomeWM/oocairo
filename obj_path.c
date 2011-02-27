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
path_gc (lua_State *L) {
    cairo_path_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATH);
    cairo_path_destroy(*obj);
    *obj = 0;
    return 0;
}

static int
path_each_iter (lua_State *L) {
    cairo_path_t *path = *(cairo_path_t **) luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATH);
    cairo_path_data_t *data;
    int i;

    if (lua_isnoneornil(L, 2))
        i = 0;
    else {
        i = luaL_checkint(L, 2);
        luaL_argcheck(L, i >= 0 && i < path->num_data, 2,
                      "path index out of range");
        i += path->data[i].header.length;
    }
    if (i >= path->num_data)
        return 0;

    lua_pushinteger(L, i);
    data = &path->data[i];
    switch (data->header.type) {
        case CAIRO_PATH_MOVE_TO:
            lua_pushliteral(L, "move-to");
            lua_createtable(L, 2, 0);
            lua_pushnumber(L, data[1].point.x);
            lua_rawseti(L, -2, 1);
            lua_pushnumber(L, data[1].point.y);
            lua_rawseti(L, -2, 2);
            break;
        case CAIRO_PATH_LINE_TO:
            lua_pushliteral(L, "line-to");
            lua_createtable(L, 2, 0);
            lua_pushnumber(L, data[1].point.x);
            lua_rawseti(L, -2, 1);
            lua_pushnumber(L, data[1].point.y);
            lua_rawseti(L, -2, 2);
            break;
        case CAIRO_PATH_CURVE_TO:
            lua_pushliteral(L, "curve-to");
            lua_createtable(L, 2, 0);
            lua_pushnumber(L, data[1].point.x);
            lua_rawseti(L, -2, 1);
            lua_pushnumber(L, data[1].point.y);
            lua_rawseti(L, -2, 2);
            lua_pushnumber(L, data[2].point.x);
            lua_rawseti(L, -2, 3);
            lua_pushnumber(L, data[2].point.y);
            lua_rawseti(L, -2, 4);
            lua_pushnumber(L, data[3].point.x);
            lua_rawseti(L, -2, 5);
            lua_pushnumber(L, data[3].point.y);
            lua_rawseti(L, -2, 6);
            break;
        case CAIRO_PATH_CLOSE_PATH:
            lua_pushliteral(L, "close-path");
            lua_pushnil(L);
            break;
        default:
            assert(0);
    }
    return 3;
}

static int
path_each (lua_State *L) {
    luaL_checkudata(L, 1, OOCAIRO_MT_NAME_PATH);
    lua_pushcfunction(L, path_each_iter);
    lua_pushvalue(L, 1);
    return 2;
}

static const luaL_Reg
path_methods[] = {
    { "__gc", path_gc },
    { "each", path_each },
    { 0, 0 }
};

/* vi:set ts=4 sw=4 expandtab: */
