static int
cairmat_create (lua_State *L) {
    cairo_matrix_t mat;
    cairo_matrix_init_identity(&mat);
    create_lua_matrix(L, &mat);
    return 1;
}

static int
cairmat_invert (lua_State *L) {
    cairo_matrix_t mat;
    from_lua_matrix(L, &mat, 1);
    if (cairo_matrix_invert(&mat) == CAIRO_STATUS_INVALID_MATRIX)
        luaL_error(L, "matrix has no inverse");
    to_lua_matrix(L, &mat, 1);
    return 0;
}

static int
cairmat_multiply (lua_State *L) {
    cairo_matrix_t m1, m2;
    from_lua_matrix(L, &m1, 1);
    from_lua_matrix(L, &m2, 2);
    cairo_matrix_multiply(&m1, &m1, &m2);
    to_lua_matrix(L, &m1, 1);
    return 0;
}

static int
cairmat_rotate (lua_State *L) {
    cairo_matrix_t mat;
    from_lua_matrix(L, &mat, 1);
    cairo_matrix_rotate(&mat, luaL_checknumber(L, 2));
    to_lua_matrix(L, &mat, 1);
    return 0;
}

static int
cairmat_scale (lua_State *L) {
    cairo_matrix_t mat;
    from_lua_matrix(L, &mat, 1);
    cairo_matrix_scale(&mat, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
    to_lua_matrix(L, &mat, 1);
    return 0;
}

static int
cairmat_transform_distance (lua_State *L) {
    cairo_matrix_t mat;
    double x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    from_lua_matrix(L, &mat, 1);
    cairo_matrix_transform_distance(&mat, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

static int
cairmat_transform_point (lua_State *L) {
    cairo_matrix_t mat;
    double x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    from_lua_matrix(L, &mat, 1);
    cairo_matrix_transform_point(&mat, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

static int
cairmat_translate (lua_State *L) {
    cairo_matrix_t mat;
    from_lua_matrix(L, &mat, 1);
    cairo_matrix_translate(&mat, luaL_checknumber(L, 2),
                           luaL_checknumber(L, 3));
    to_lua_matrix(L, &mat, 1);
    return 0;
}

static const luaL_Reg
cairmat_methods[] = {
    { "invert", cairmat_invert },
    { "multiply", cairmat_multiply },
    { "rotate", cairmat_rotate },
    { "scale", cairmat_scale },
    { "transform_distance", cairmat_transform_distance },
    { "transform_point", cairmat_transform_point },
    { "translate", cairmat_translate },
    { 0, 0 }
};

/* vi:set ts=4 sw=4 expandtab: */
