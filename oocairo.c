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

#include "oocairo.h"
#include <cairo.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef CAIRO_HAS_PDF_SURFACE
#include <cairo-pdf.h>
#endif
#ifdef CAIRO_HAS_PS_SURFACE
#include <cairo-ps.h>
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
#include <cairo-svg.h>
#endif

#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1, 6, 0)
#error "This Lua binding requires Cairo version 1.6 or better."
#endif

#if LUA_VERSION_NUM >= 502
#define lua_objlen(a, b)      lua_rawlen(a, b)

static int luaL_typerror (lua_State *L, int narg, const char *tname) {
    const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                      tname, luaL_typename(L, narg));
    return luaL_argerror(L, narg, msg);
}
#else
#define lua_rawlen(a, b) do_not_use_lua_rawlen_use_lua_objlen_instead;
#endif

#if LUA_VERSION_NUM >= 503
#define oocairo_lua_isinteger(L, idx) lua_isinteger(L, idx)
#else
#define oocairo_lua_isinteger(L, idx) lua_isnumber(L, idx)
#endif

static const int ENDIANNESS_TEST_VAL = 1;
#define IS_BIG_ENDIAN (!(*(const char *) &ENDIANNESS_TEST_VAL))

#define ENUM_VAL_TO_LUA_STRING_FUNC(type) \
    static int \
    type ## _to_lua (lua_State *L, cairo_ ## type ## _t val) { \
        int i; \
        for (i = 0; type ## _names[i]; ++i) { \
            if (val == type ## _values[i]) { \
                lua_pushstring(L, type ## _names[i]); \
                return 1; \
            } \
        } \
        return 0; \
    }
#define ENUM_VAL_FROM_LUA_STRING_FUNC(type) \
    static cairo_ ## type ## _t \
    type ## _from_lua (lua_State *L, int pos) { \
        return type ## _values[luaL_checkoption(L, pos, 0, type ## _names)]; \
    }

static const char * const format_names[] = {
    "argb32", "rgb24", "a8", "a1", 0
};
static const cairo_format_t format_values[] = {
    CAIRO_FORMAT_ARGB32, CAIRO_FORMAT_RGB24, CAIRO_FORMAT_A8, CAIRO_FORMAT_A1
};
ENUM_VAL_TO_LUA_STRING_FUNC(format)
ENUM_VAL_FROM_LUA_STRING_FUNC(format)

static const char * const antialias_names[] = {
    "default", "none", "gray", "subpixel", 0
};
static const cairo_antialias_t antialias_values[] = {
    CAIRO_ANTIALIAS_DEFAULT, CAIRO_ANTIALIAS_NONE, CAIRO_ANTIALIAS_GRAY,
    CAIRO_ANTIALIAS_SUBPIXEL
};
ENUM_VAL_TO_LUA_STRING_FUNC(antialias)

static cairo_antialias_t
antialias_from_lua (lua_State *L, int pos) {
    if (lua_isboolean(L, pos))
        /* A boolean value is interpreted as a signal to turn AA on or off. */
        return lua_toboolean(L, pos) ? CAIRO_ANTIALIAS_DEFAULT
                                     : CAIRO_ANTIALIAS_NONE;
    else
        return antialias_values[luaL_checkoption(L, pos, 0, antialias_names)];
}

static const char * const subpixel_order_names[] = {
    "default", "rgb", "bgr", "vrgb", "vbgr", 0
};
static const cairo_subpixel_order_t subpixel_order_values[] = {
    CAIRO_SUBPIXEL_ORDER_DEFAULT,
    CAIRO_SUBPIXEL_ORDER_RGB,  CAIRO_SUBPIXEL_ORDER_BGR,
    CAIRO_SUBPIXEL_ORDER_VRGB, CAIRO_SUBPIXEL_ORDER_VBGR
};
ENUM_VAL_TO_LUA_STRING_FUNC(subpixel_order)
ENUM_VAL_FROM_LUA_STRING_FUNC(subpixel_order)

static const char * const hint_style_names[] = {
    "default", "none", "slight", "medium", "full", 0
};
static const cairo_hint_style_t hint_style_values[] = {
    CAIRO_HINT_STYLE_DEFAULT, CAIRO_HINT_STYLE_NONE,
    CAIRO_HINT_STYLE_SLIGHT, CAIRO_HINT_STYLE_MEDIUM, CAIRO_HINT_STYLE_FULL
};
ENUM_VAL_TO_LUA_STRING_FUNC(hint_style)
ENUM_VAL_FROM_LUA_STRING_FUNC(hint_style)

static const char * const hint_metrics_names[] = {
    "default", "off", "on", 0
};
static const cairo_hint_metrics_t hint_metrics_values[] = {
    CAIRO_HINT_METRICS_DEFAULT, CAIRO_HINT_METRICS_OFF, CAIRO_HINT_METRICS_ON
};
ENUM_VAL_TO_LUA_STRING_FUNC(hint_metrics)
ENUM_VAL_FROM_LUA_STRING_FUNC(hint_metrics)

static const char * const line_cap_names[] = {
    "butt", "round", "square", 0
};
static const cairo_line_cap_t line_cap_values[] = {
    CAIRO_LINE_CAP_BUTT, CAIRO_LINE_CAP_ROUND, CAIRO_LINE_CAP_SQUARE
};
ENUM_VAL_TO_LUA_STRING_FUNC(line_cap)
ENUM_VAL_FROM_LUA_STRING_FUNC(line_cap)

static const char * const line_join_names[] = {
    "miter", "round", "bevel", 0
};
static const cairo_line_join_t line_join_values[] = {
    CAIRO_LINE_JOIN_MITER, CAIRO_LINE_JOIN_ROUND, CAIRO_LINE_JOIN_BEVEL
};
ENUM_VAL_TO_LUA_STRING_FUNC(line_join)
ENUM_VAL_FROM_LUA_STRING_FUNC(line_join)

static const char * const fill_rule_names[] = {
    "winding", "even-odd", 0
};
static const cairo_fill_rule_t fill_rule_values[] = {
    CAIRO_FILL_RULE_WINDING, CAIRO_FILL_RULE_EVEN_ODD
};
ENUM_VAL_TO_LUA_STRING_FUNC(fill_rule)
ENUM_VAL_FROM_LUA_STRING_FUNC(fill_rule)

static const char * const operator_names[] = {
    "clear",
    "source", "over", "in", "out", "atop",
    "dest", "dest-over", "dest-in", "dest-out", "dest-atop",
    "xor", "add", "saturate",
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
    "multiply", "screen", "overlay", "darken", "lighten", "color-dodge",
    "color-burn", "hard-light", "soft-light", "difference", "exclusion",
    "hsl-hue", "hsl-saturation", "hsl-color", "hsl-luminosity",
#endif
    0
};
static const cairo_operator_t operator_values[] = {
    CAIRO_OPERATOR_CLEAR,
    CAIRO_OPERATOR_SOURCE, CAIRO_OPERATOR_OVER, CAIRO_OPERATOR_IN,
    CAIRO_OPERATOR_OUT, CAIRO_OPERATOR_ATOP,
    CAIRO_OPERATOR_DEST, CAIRO_OPERATOR_DEST_OVER, CAIRO_OPERATOR_DEST_IN,
    CAIRO_OPERATOR_DEST_OUT, CAIRO_OPERATOR_DEST_ATOP,
    CAIRO_OPERATOR_XOR, CAIRO_OPERATOR_ADD,
#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1, 10, 0)
    CAIRO_OPERATOR_SATURATE
#else
    CAIRO_OPERATOR_SATURATE, CAIRO_OPERATOR_MULTIPLY, CAIRO_OPERATOR_SCREEN,
    CAIRO_OPERATOR_OVERLAY, CAIRO_OPERATOR_DARKEN, CAIRO_OPERATOR_LIGHTEN,
    CAIRO_OPERATOR_COLOR_DODGE, CAIRO_OPERATOR_COLOR_BURN, CAIRO_OPERATOR_HARD_LIGHT,
    CAIRO_OPERATOR_SOFT_LIGHT, CAIRO_OPERATOR_DIFFERENCE, CAIRO_OPERATOR_EXCLUSION,
    CAIRO_OPERATOR_HSL_HUE, CAIRO_OPERATOR_HSL_SATURATION, CAIRO_OPERATOR_HSL_COLOR,
    CAIRO_OPERATOR_HSL_LUMINOSITY
#endif
};
ENUM_VAL_TO_LUA_STRING_FUNC(operator)
ENUM_VAL_FROM_LUA_STRING_FUNC(operator)

static const char * const content_names[] = {
    "color", "alpha", "color-alpha", 0
};
static const cairo_content_t content_values[] = {
    CAIRO_CONTENT_COLOR, CAIRO_CONTENT_ALPHA, CAIRO_CONTENT_COLOR_ALPHA
};
ENUM_VAL_TO_LUA_STRING_FUNC(content)
ENUM_VAL_FROM_LUA_STRING_FUNC(content)

static const char * const extend_names[] = {
    "none", "repeat", "reflect", "pad", 0
};
static const cairo_extend_t extend_values[] = {
    CAIRO_EXTEND_NONE, CAIRO_EXTEND_REPEAT, CAIRO_EXTEND_REFLECT,
    CAIRO_EXTEND_PAD
};
ENUM_VAL_TO_LUA_STRING_FUNC(extend)
ENUM_VAL_FROM_LUA_STRING_FUNC(extend)

static const char * const filter_names[] = {
    "fast", "good", "best", "nearest", "bilinear", "gaussian", 0
};
static const cairo_filter_t filter_values[] = {
    CAIRO_FILTER_FAST, CAIRO_FILTER_GOOD, CAIRO_FILTER_BEST,
    CAIRO_FILTER_NEAREST, CAIRO_FILTER_BILINEAR, CAIRO_FILTER_GAUSSIAN
};
ENUM_VAL_TO_LUA_STRING_FUNC(filter)
ENUM_VAL_FROM_LUA_STRING_FUNC(filter)

static const char * const font_slant_names[] = {
    "normal", "italic", "oblique", 0
};
static const cairo_font_slant_t font_slant_values[] = {
    CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_SLANT_OBLIQUE
};
ENUM_VAL_TO_LUA_STRING_FUNC(font_slant)
ENUM_VAL_FROM_LUA_STRING_FUNC(font_slant)

static const char * const font_weight_names[] = {
    "normal", "bold", 0
};
static const cairo_font_weight_t font_weight_values[] = {
  CAIRO_FONT_WEIGHT_NORMAL, CAIRO_FONT_WEIGHT_BOLD
};
ENUM_VAL_TO_LUA_STRING_FUNC(font_weight)
ENUM_VAL_FROM_LUA_STRING_FUNC(font_weight)

static const char * const font_type_names[] = {
    "toy", "ft", "win32", "quartz",
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    "user",
#endif
    0
};
static const cairo_font_type_t font_type_values[] = {
    CAIRO_FONT_TYPE_TOY, CAIRO_FONT_TYPE_FT,
    CAIRO_FONT_TYPE_WIN32, CAIRO_FONT_TYPE_QUARTZ,
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    CAIRO_FONT_TYPE_USER,
#endif
};
ENUM_VAL_TO_LUA_STRING_FUNC(font_type)

static const char * const surface_type_names[] = {
    "image", "pdf", "ps",
    "xlib", "xcb", "glitz",
    "quartz", "win32",
    "beos", "directfb",
    "svg", "os2",
    "win32-printing", "quartz-image",
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
    "script", "qt", "recording", "vg", "gl", "drm", "tee",
    "xml", "skia", "subsurface",
#endif
    0
};
static const cairo_surface_type_t surface_type_values[] = {
    CAIRO_SURFACE_TYPE_IMAGE, CAIRO_SURFACE_TYPE_PDF, CAIRO_SURFACE_TYPE_PS,
    CAIRO_SURFACE_TYPE_XLIB, CAIRO_SURFACE_TYPE_XCB, CAIRO_SURFACE_TYPE_GLITZ,
    CAIRO_SURFACE_TYPE_QUARTZ, CAIRO_SURFACE_TYPE_WIN32,
    CAIRO_SURFACE_TYPE_BEOS, CAIRO_SURFACE_TYPE_DIRECTFB,
    CAIRO_SURFACE_TYPE_SVG, CAIRO_SURFACE_TYPE_OS2,
    CAIRO_SURFACE_TYPE_WIN32_PRINTING,
#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1, 10, 0)
    CAIRO_SURFACE_TYPE_QUARTZ_IMAGE
#else
    CAIRO_SURFACE_TYPE_QUARTZ_IMAGE, CAIRO_SURFACE_TYPE_SCRIPT,
    CAIRO_SURFACE_TYPE_QT, CAIRO_SURFACE_TYPE_RECORDING, CAIRO_SURFACE_TYPE_VG,
    CAIRO_SURFACE_TYPE_GL, CAIRO_SURFACE_TYPE_DRM, CAIRO_SURFACE_TYPE_TEE,
    CAIRO_SURFACE_TYPE_XML, CAIRO_SURFACE_TYPE_SKIA,
    CAIRO_SURFACE_TYPE_SUBSURFACE,
#endif
};
ENUM_VAL_TO_LUA_STRING_FUNC(surface_type)

#if defined(CAIRO_HAS_PDF_SURFACE) && CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
static const char *pdf_version_names[] = {
    "1.4", "1.5", 0
};
static const cairo_pdf_version_t pdf_version_values[] = {
    CAIRO_PDF_VERSION_1_4, CAIRO_PDF_VERSION_1_5
};
ENUM_VAL_FROM_LUA_STRING_FUNC(pdf_version)
#endif

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
static const char *region_overlap_names[] = {
    "overlap-in", "overlap-out", "overlap-part", 0
};
static const cairo_region_overlap_t region_overlap_values[] = {
    CAIRO_REGION_OVERLAP_IN, CAIRO_REGION_OVERLAP_OUT, CAIRO_REGION_OVERLAP_PART
};
ENUM_VAL_TO_LUA_STRING_FUNC(region_overlap)
#endif

static void
to_lua_matrix (lua_State *L, cairo_matrix_t *mat, int pos) {
    double *matnums;
    int i;
    matnums = (double *) mat;
    for (i = 0; i < 6; ++i) {
        lua_pushnumber(L, matnums[i]);
        lua_rawseti(L, pos, i + 1);
    }
}

static void
create_lua_matrix (lua_State *L, cairo_matrix_t *mat) {
    lua_createtable(L, 6, 0);
    to_lua_matrix(L, mat, lua_gettop(L));
    luaL_getmetatable(L, OOCAIRO_MT_NAME_MATRIX);
    lua_setmetatable(L, -2);
}

static void
from_lua_matrix (lua_State *L, cairo_matrix_t *mat, int pos) {
    double *matnums;
    int i;
    luaL_checktype(L, pos, LUA_TTABLE);
    matnums = (double *) mat;
    for (i = 0; i < 6; ++i) {
        lua_rawgeti(L, pos, i + 1);
        if (!lua_isnumber(L, -1))
            luaL_error(L, "value %d in matrix isn't a number", i + 1);
        matnums[i] = lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
}

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
static int
to_lua_rectangle (lua_State *L, cairo_rectangle_int_t *rect) {
    lua_newtable(L);
#define DO(t) \
    lua_pushstring(L, #t); \
    lua_pushnumber(L, rect->t); \
    lua_settable(L, -3)
    DO(x);
    DO(y);
    DO(width);
    DO(height);
#undef DO
    return 1;
}

static void
from_lua_rectangle (lua_State *L, cairo_rectangle_int_t *rect, int pos) {
    luaL_checktype(L, pos, LUA_TTABLE);
#define DO(t) \
    lua_pushstring(L, #t); \
    lua_gettable(L, pos); \
    if (!oocairo_lua_isinteger(L, -1)) { \
        luaL_argerror(L, pos, "invalid rectangle: coordinates must be integer values"); \
    } \
    rect->t = lua_tointeger(L, -1); \
    lua_pop(L, 1)
    DO(x);
    DO(y);
    DO(width);
    DO(height);
#undef DO
}

static void
from_lua_rectangle_list_element (lua_State *L, cairo_rectangle_int_t *rect, int list_pos, int element_index) {
    lua_rawgeti(L, list_pos, element_index); /* push rectangle from list */
    if (lua_type(L, -1) != LUA_TTABLE) {
        luaL_argerror(L, list_pos, "list contains invalid rectangle");
    }
#define DO(t) \
    lua_pushstring(L, #t); \
    lua_gettable(L, -2); \
    if (!oocairo_lua_isinteger(L, -1)) { \
        luaL_argerror(L, list_pos, "list contains invalid rectangle: coordinates must be integer values"); \
    } \
    rect->t = lua_tointeger(L, -1); \
    lua_pop(L, 1)
    DO(x);
    DO(y);
    DO(width);
    DO(height);
    lua_pop(L, 1); /* pop rectangle */
#undef DO
}
#endif

static void
create_lua_font_extents (lua_State *L, cairo_font_extents_t *extents) {
    lua_createtable(L, 0, 5);
    lua_pushliteral(L, "ascent");
    lua_pushnumber(L, extents->ascent);
    lua_rawset(L, -3);
    lua_pushliteral(L, "descent");
    lua_pushnumber(L, extents->descent);
    lua_rawset(L, -3);
    lua_pushliteral(L, "height");
    lua_pushnumber(L, extents->height);
    lua_rawset(L, -3);
    lua_pushliteral(L, "max_x_advance");
    lua_pushnumber(L, extents->max_x_advance);
    lua_rawset(L, -3);
    lua_pushliteral(L, "max_y_advance");
    lua_pushnumber(L, extents->max_y_advance);
    lua_rawset(L, -3);
}

#define HANDLE_FONT_EXTENTS_FIELD(name) \
    lua_pushliteral(L, #name); \
    lua_rawget(L, -2); \
    if (!lua_isnumber(L, -1)) \
        luaL_error(L, "font extents entry '" #name "' must be a number"); \
    extents->name = lua_tonumber(L, -1); \
    lua_pop(L, 1);
static void
from_lua_font_extents (lua_State *L, cairo_font_extents_t *extents) {
    if (!lua_istable(L, -1))
        luaL_error(L, "font extents value must be a table");
    HANDLE_FONT_EXTENTS_FIELD(ascent)
    HANDLE_FONT_EXTENTS_FIELD(descent)
    HANDLE_FONT_EXTENTS_FIELD(height)
    HANDLE_FONT_EXTENTS_FIELD(max_x_advance)
    HANDLE_FONT_EXTENTS_FIELD(max_y_advance)
}
#undef HANDLE_FONT_EXTENTS_FIELD

static void
create_lua_text_extents (lua_State *L, cairo_text_extents_t *extents) {
    lua_createtable(L, 0, 6);
    lua_pushliteral(L, "x_bearing");
    lua_pushnumber(L, extents->x_bearing);
    lua_rawset(L, -3);
    lua_pushliteral(L, "y_bearing");
    lua_pushnumber(L, extents->y_bearing);
    lua_rawset(L, -3);
    lua_pushliteral(L, "width");
    lua_pushnumber(L, extents->width);
    lua_rawset(L, -3);
    lua_pushliteral(L, "height");
    lua_pushnumber(L, extents->height);
    lua_rawset(L, -3);
    lua_pushliteral(L, "x_advance");
    lua_pushnumber(L, extents->x_advance);
    lua_rawset(L, -3);
    lua_pushliteral(L, "y_advance");
    lua_pushnumber(L, extents->y_advance);
    lua_rawset(L, -3);
}

#define HANDLE_TEXT_EXTENTS_FIELD(name) \
    lua_pushliteral(L, #name); \
    lua_rawget(L, -2); \
    if (!lua_isnumber(L, -1)) \
        luaL_error(L, "text extents entry '" #name "' must be a number"); \
    extents->name = lua_tonumber(L, -1); \
    lua_pop(L, 1);
static void
from_lua_text_extents (lua_State *L, cairo_text_extents_t *extents) {
    if (!lua_istable(L, -1))
        luaL_error(L, "text extents value must be a table");
    HANDLE_TEXT_EXTENTS_FIELD(x_bearing)
    HANDLE_TEXT_EXTENTS_FIELD(y_bearing)
    HANDLE_TEXT_EXTENTS_FIELD(width)
    HANDLE_TEXT_EXTENTS_FIELD(height)
    HANDLE_TEXT_EXTENTS_FIELD(x_advance)
    HANDLE_TEXT_EXTENTS_FIELD(y_advance)
}
#undef HANDLE_TEXT_EXTENTS_FIELD

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
#define GLYPHS_ALLOC(num) cairo_glyph_allocate(num)
#define GLYPHS_FREE(glyphs) cairo_glyph_free(glyphs)
#else
#define GLYPHS_ALLOC(num) malloc(sizeof(cairo_glyph_t) * (num))
#define GLYPHS_FREE(glyphs) free(glyphs)
#endif

static void
create_lua_glyph_array (lua_State *L, cairo_glyph_t *glyphs, int num_glyphs) {
    int i;
    lua_createtable(L, num_glyphs, 0);

    for (i = 0; i < num_glyphs; ++i) {
        lua_createtable(L, 3, 0);
        lua_pushnumber(L, glyphs[i].index);
        lua_rawseti(L, -2, 1);
        lua_pushnumber(L, glyphs[i].x);
        lua_rawseti(L, -2, 2);
        lua_pushnumber(L, glyphs[i].y);
        lua_rawseti(L, -2, 3);
        lua_rawseti(L, -2, i + 1);
    }
}

static void
from_lua_glyph_array (lua_State *L, cairo_glyph_t **glyphs, int *num_glyphs,
                      int pos)
{
    int i;
    double n;

    luaL_checktype(L, pos, LUA_TTABLE);
    *num_glyphs = lua_objlen(L, pos);
    if (*num_glyphs == 0) {
        *glyphs = 0;
        return;
    }
    *glyphs = GLYPHS_ALLOC(*num_glyphs);
    if (!*glyphs) {
        luaL_error(L, "out of memory");
        return;
    }

    for (i = 0; i < *num_glyphs; ++i) {
        lua_rawgeti(L, pos, i + 1);
        if (!lua_istable(L, -1)) {
            free(*glyphs);
            luaL_error(L, "glyph %d is not a table", i + 1);
        }
        else if (lua_objlen(L, -1) != 3) {
            free(*glyphs);
            luaL_error(L, "glyph %d should contain exactly 3 numbers", i + 1);
        }
        lua_rawgeti(L, -1, 1);
        if (!lua_isnumber(L, -1)) {
            free(*glyphs);
            luaL_error(L, "index of glyph %d should be a number", i + 1);
        }
        n = lua_tonumber(L, -1);
        if (n < 0) {
            free(*glyphs);
            luaL_error(L, "index number of glyph %d is negative", i + 1);
        }
        (*glyphs)[i].index = (unsigned long) n;
        lua_pop(L, 1);
        lua_rawgeti(L, -1, 2);
        if (!lua_isnumber(L, -1)) {
            free(*glyphs);
            luaL_error(L, "x position for glyph %d should be a number", i + 1);
        }
        (*glyphs)[i].x = lua_tonumber(L, -1);
        lua_pop(L, 1);
        lua_rawgeti(L, -1, 3);
        if (!lua_isnumber(L, -1)) {
            free(*glyphs);
            luaL_error(L, "y position for glyph %d should be a number", i + 1);
        }
        (*glyphs)[i].y = lua_tonumber(L, -1);
        lua_pop(L, 2);
    }
}

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static void
create_lua_text_cluster_table (lua_State *L, cairo_text_cluster_t *clusters,
                               int num, cairo_text_cluster_flags_t flags)
{
    int i;
    lua_createtable(L, num, 1);

    lua_pushliteral(L, "backward");
    lua_pushboolean(L, flags & CAIRO_TEXT_CLUSTER_FLAG_BACKWARD);
    lua_rawset(L, -3);

    for (i = 0; i < num; ++i) {
        lua_createtable(L, 2, 0);
        lua_pushnumber(L, clusters[i].num_bytes);
        lua_rawseti(L, -2, 1);
        lua_pushnumber(L, clusters[i].num_glyphs);
        lua_rawseti(L, -2, 2);
        lua_rawseti(L, -2, i + 1);
    }
}

static void
from_lua_clusters_table (lua_State *L, cairo_text_cluster_t **clusters,
                         int *num, cairo_text_cluster_flags_t *flags, int pos)
{
    int i;
    int n;
    luaL_checktype(L, pos, LUA_TTABLE);

    *flags = 0;
    lua_pushliteral(L, "backward");
    lua_getfield(L, -1, "backward");
    if (lua_toboolean(L, -1))
        *flags |= CAIRO_TEXT_CLUSTER_FLAG_BACKWARD;
    lua_pop(L, 1);

    *num = lua_objlen(L, pos);
    if (*num == 0) {
        *clusters = 0;
        return;
    }
    *clusters = cairo_text_cluster_allocate(*num);
    if (!*clusters) {
        luaL_error(L, "out of memory");
        return;
    }

    for (i = 0; i < *num; ++i) {
        lua_rawgeti(L, pos, i + 1);
        if (!lua_istable(L, -1)) {
            free(*clusters);
            luaL_error(L, "text cluster %d is not a table", i + 1);
        }
        else if (lua_objlen(L, -1) != 2) {
            free(*clusters);
            luaL_error(L, "text cluster %d should contain exactly 2 numbers",
                       i + 1);
        }

        lua_rawgeti(L, -1, 1);
        if (!lua_isnumber(L, -1)) {
            free(*clusters);
            luaL_error(L, "number of bytes of text cluster %d should be a"
                       " number", i + 1);
        }
        n = lua_tonumber(L, -1);
        if (n < 0) {
            free(*clusters);
            luaL_error(L, "number of bytes of text cluster %d is negative",
                       i + 1);
        }
        (*clusters)[i].num_bytes = n;
        lua_pop(L, 1);

        lua_rawgeti(L, -1, 2);
        if (!lua_isnumber(L, -1)) {
            free(*clusters);
            luaL_error(L, "number of glyphs of text cluster %d should be a"
                       " number", i + 1);
        }
        n = lua_tonumber(L, -1);
        if (n < 0) {
            free(*clusters);
            luaL_error(L, "number of glyphs of text cluster %d is negative",
                       i + 1);
        }
        (*clusters)[i].num_glyphs = n;
        lua_pop(L, 2);
    }
}
#endif

typedef struct SurfaceUserdata_ {
    /* This has to be first, because most users of this ignore the rest and
     * just treat a pointer to this structure as if it was a pointer to the
     * surface pointer. */
    cairo_surface_t *surface;
    /* This stuff is only used for surfaces which are continuously written
     * to a file handle during their lifetime. */
    lua_State *L;
    int fhref;
    const char *errmsg;
    int errmsg_free;        /* true if errmsg must be freed */
    /* This is only used for image surfaces initialized from data.  A copy
     * is made and referenced here, and only freed when the surface object
     * is GCed. */
    unsigned char *image_buffer;
} SurfaceUserdata;

static void
init_surface_userdata (lua_State *L, SurfaceUserdata *ud) {
    ud->surface = 0;
    ud->L = L;
    ud->fhref = LUA_NOREF;
    ud->errmsg = 0;
    ud->errmsg_free = 0;
    ud->image_buffer = 0;
}

static cairo_pattern_t **
create_pattern_userdata (lua_State *L) {
    cairo_pattern_t **obj = lua_newuserdata(L, sizeof(cairo_pattern_t *));
    *obj = 0;
    luaL_getmetatable(L, OOCAIRO_MT_NAME_PATTERN);
    lua_setmetatable(L, -2);
    return obj;
}

static cairo_font_face_t **
create_fontface_userdata (lua_State *L) {
    cairo_font_face_t **obj = lua_newuserdata(L, sizeof(cairo_font_face_t *));
    *obj = 0;
    luaL_getmetatable(L, OOCAIRO_MT_NAME_FONTFACE);
    lua_setmetatable(L, -2);
    return obj;
}

static cairo_scaled_font_t **
create_scaledfont_userdata (lua_State *L) {
    cairo_scaled_font_t **obj
            = lua_newuserdata(L, sizeof(cairo_scaled_font_t *));
    *obj = 0;
    luaL_getmetatable(L, OOCAIRO_MT_NAME_SCALEDFONT);
    lua_setmetatable(L, -2);
    return obj;
}

static cairo_font_options_t **
create_fontopt_userdata (lua_State *L) {
    cairo_font_options_t **obj
            = lua_newuserdata(L, sizeof(cairo_font_options_t *));
    *obj = 0;
    luaL_getmetatable(L, OOCAIRO_MT_NAME_FONTOPT);
    lua_setmetatable(L, -2);
    return obj;
}

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
static cairo_region_t **
create_region_userdata (lua_State *L) {
    cairo_region_t **obj = lua_newuserdata(L, sizeof(cairo_region_t *));
    *obj = 0;
    luaL_getmetatable(L, OOCAIRO_MT_NAME_REGION);
    lua_setmetatable(L, -2);
    return obj;
}
#endif

static cairo_t **
create_context_userdata (lua_State *L) {
    cairo_t **obj = lua_newuserdata(L, sizeof(cairo_t *));
    *obj = 0;
    luaL_getmetatable(L, OOCAIRO_MT_NAME_CONTEXT);
    lua_setmetatable(L, -2);
    return obj;
}

static SurfaceUserdata *
create_surface_userdata (lua_State *L) {
    SurfaceUserdata *ud = lua_newuserdata(L, sizeof(SurfaceUserdata));
    init_surface_userdata(L, ud);
    luaL_getmetatable(L, OOCAIRO_MT_NAME_SURFACE);
    lua_setmetatable(L, -2);
    return ud;
}

#define PUSH(name, type, func, reference) \
int \
oocairo_ ## name ## _push (lua_State *L, type *obj) \
{ \
    type **o = create_ ## func ## _userdata(L); \
    *o = reference(obj); \
    return 1; \
}

PUSH(pattern, cairo_pattern_t, pattern, cairo_pattern_reference)
PUSH(font_face, cairo_font_face_t, fontface, cairo_font_face_reference)
PUSH(scaled_font, cairo_scaled_font_t, scaledfont, cairo_scaled_font_reference)
PUSH(font_options, cairo_font_options_t, fontopt, cairo_font_options_copy)
PUSH(context, cairo_t, context, cairo_reference)
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
PUSH(region, cairo_region_t, region, cairo_region_reference)
#endif

int
oocairo_surface_push (lua_State *L, cairo_surface_t *surface)
{
    SurfaceUserdata *ud;
    ud = create_surface_userdata(L);
    ud->surface = cairo_surface_reference(surface);
    return 1;
}

int
oocairo_matrix_push (lua_State *L, cairo_matrix_t *matrix)
{
    create_lua_matrix(L, matrix);
    return 1;
}

static void
free_surface_userdata (SurfaceUserdata *ud) {
    if (ud->surface) {
        cairo_surface_destroy(ud->surface);
        ud->surface = 0;
    }
    if (ud->fhref != LUA_NOREF) {
        luaL_unref(ud->L, LUA_REGISTRYINDEX, ud->fhref);
        ud->fhref = LUA_NOREF;
    }
    if (ud->errmsg) {
        if (ud->errmsg_free)
            free((char *) ud->errmsg);
        ud->errmsg = 0;
        ud->errmsg_free = 0;
    }
    if (ud->image_buffer) {
        free(ud->image_buffer);
        ud->image_buffer = 0;
    }
}

static char *
my_strdup (const char *s) {
    char *copy = malloc(strlen(s) + 1);
    if (copy) {
        strcpy(copy, s);
    }
    return copy;
}

static cairo_status_t
write_chunk_to_fh (void *closure, const unsigned char *buf,
                   unsigned int lentowrite)
{
    SurfaceUserdata *info = closure;
    lua_State *L = info->L;

    lua_rawgeti(L, LUA_REGISTRYINDEX, info->fhref);
    lua_getfield(L, -1, "write");
    if (lua_isnil(L, -1)) {
        info->errmsg = "file handle does not have 'write' method";
        lua_pop(L, 2);
        return CAIRO_STATUS_WRITE_ERROR;
    }
    lua_pushvalue(L, -2);
    lua_pushlstring(L, (const char *) buf, lentowrite);
    if (lua_pcall(L, 2, 0, 0)) {
        if (lua_isstring(L, -1)) {
            info->errmsg = my_strdup(lua_tostring(L, -1));
            info->errmsg_free = 1;
        }
        lua_pop(L, 1);
        return CAIRO_STATUS_WRITE_ERROR;
    }

    lua_pop(L, 1);
    return CAIRO_STATUS_SUCCESS;
}

#if LUA_VERSION_NUM >= 502
static void
get_gtk_module_function (lua_State *L, const char *name) {
    /* FIXME: Implement this again */
    (void) name;
    luaL_error(L, "cannot access gtk with lua >= 5.2, TODO: Implement this again");
}
#else
static void
get_gtk_module_function (lua_State *L, const char *name) {
    lua_getfield(L, LUA_GLOBALSINDEX, "gtk");
    if (lua_isnil(L, -1))
        luaL_error(L, "no global variable 'gtk', you need to load the module"
                   " with require'gtk' before using this function");
    lua_getfield(L, -1, name);
    if (lua_isnil(L, -1))
        luaL_error(L, "could not find '%s' function in 'gtk' module table",
                   name);
    lua_remove(L, -2);
}
#endif

static int
push_cairo_status (lua_State *L, cairo_status_t status) {
    if (status == CAIRO_STATUS_SUCCESS)
        return 0;
    lua_pushstring(L, cairo_status_to_string(status));
    return 1;
}

#include "obj_context.c"
#include "obj_font_face.c"
#include "obj_font_opt.c"
#include "obj_matrix.c"
#include "obj_path.c"
#include "obj_pattern.c"
#include "obj_scaled_font.c"
#include "obj_surface.c"
#include "obj_region.c"

static int
format_stride_for_width (lua_State *L) {
    cairo_format_t fmt = format_from_lua(L, 1);
    int width = luaL_checknumber(L, 2);
    lua_pushnumber(L, cairo_format_stride_for_width(fmt, width));
    return 1;
}

#define CHECK_VER(func, version)                                  \
static int                                                        \
func (lua_State *L) {                                             \
    int major = luaL_checknumber(L, 1);                           \
    int minor = luaL_checknumber(L, 2);                           \
    int micro = luaL_checknumber(L, 3);                           \
                                                                  \
    if ((version) >= CAIRO_VERSION_ENCODE(major, minor, micro)) { \
        lua_pushboolean(L, 1);                                    \
    } else {                                                      \
        lua_pushboolean(L, 0);                                    \
    }                                                             \
    return 1;                                                     \
}

CHECK_VER(check_version, CAIRO_VERSION)
CHECK_VER(check_runtime_version, cairo_version())

static const luaL_Reg
constructor_funcs[] = {
    { "check_version", check_version },
    { "check_runtime_version", check_runtime_version },
    { "context_create", context_create },
    { "context_create_gdk", context_create_gdk },
    { "font_options_create", font_options_create },
    { "format_stride_for_width", format_stride_for_width },
    { "image_surface_create", image_surface_create },
    { "image_surface_create_from_data", image_surface_create_from_data },
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    { "image_surface_create_from_png", image_surface_create_from_png },
#endif
    { "matrix_create", cairmat_create },
    { "pattern_create_for_surface", pattern_create_for_surface },
    { "pattern_create_linear", pattern_create_linear },
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 12, 0)
    { "pattern_create_mesh", pattern_create_mesh },
#endif
    { "pattern_create_radial", pattern_create_radial },
    { "pattern_create_rgb", pattern_create_rgb },
    { "pattern_create_rgba", pattern_create_rgba },
#ifdef CAIRO_HAS_PDF_SURFACE
    { "pdf_surface_create", pdf_surface_create },
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    { "ps_get_levels", ps_get_levels },
    { "ps_surface_create", ps_surface_create },
#endif
    { "scaled_font_create", scaled_font_create },
    { "surface_create_similar", surface_create_similar },
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 12, 0)
    { "surface_create_similar_image", surface_create_similar_image },
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
    { "svg_surface_create", svg_surface_create },
    { "svg_get_versions", svg_get_versions },
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    { "toy_font_face_create", toy_font_face_create },
#endif
#ifdef CAIRO_HAS_USER_FONT
    { "user_font_face_create", user_font_face_create },
#endif
    /* New in cairo 1.10 */
#ifdef CAIRO_HAS_RECORDING_SURFACE
    { "recording_surface_create", recording_surface_create },
    { "recording_surface_ink_extents", recording_surface_ink_extents },
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
    { "region_create", region_create },
    { "region_create_rectangle", region_create_rectangle },
    { "region_create_rectangles", region_create_rectangles },
#endif
    { 0, 0 }
};

static void
add_funcs_to_table (lua_State *L, const luaL_Reg *funcs) {
    const luaL_Reg *l;
    for (l = funcs; l->name; ++l) {
        lua_pushstring(L, l->name);
        lua_pushcfunction(L, l->func);
        lua_rawset(L, -3);
    }
}

static void
create_object_metatable (lua_State *L, const char *mt_name,
                         const char *debug_name, const luaL_Reg *methods)
{
    if (luaL_newmetatable(L, mt_name)) {
        lua_pushliteral(L, "_NAME");
        lua_pushstring(L, debug_name);
        lua_rawset(L, -3);
        lua_pushliteral(L, "__name");
        lua_pushstring(L, debug_name);
        lua_rawset(L, -3);
        add_funcs_to_table(L, methods);
        lua_pushliteral(L, "__index");
        lua_pushvalue(L, -2);
        lua_rawset(L, -3);

        if (strcmp(mt_name, OOCAIRO_MT_NAME_CONTEXT) == 0) {
            /* The MT for context objects has an extra field which we don't
             * use, but which allows the Lua-Gnome binding to recognize
             * the objects. */
            lua_pushliteral(L, "_classname");
            lua_pushstring(L, "cairo");
            lua_rawset(L, -3);
        }
    }

    lua_pop(L, 1);
}

int
luaopen_oocairo (lua_State *L) {

#ifdef VALGRIND_LUA_MODULE_HACK
    /* Hack to allow Valgrind to access debugging info for the module. */
    luaL_getmetatable(L, "_LOADLIB");
    lua_pushnil(L);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);
#endif

    /* Create the table to return from 'require' */
    lua_newtable(L);
    lua_pushliteral(L, "_NAME");
    lua_pushliteral(L, "cairo");
    lua_rawset(L, -3);
    lua_pushliteral(L, "_VERSION");
    lua_pushliteral(L, VERSION);
    lua_rawset(L, -3);
    lua_pushliteral(L, "_CAIRO_VERSION");
    lua_pushstring(L, CAIRO_VERSION_STRING);
    lua_rawset(L, -3);
    lua_pushliteral(L, "_CAIRO_RUNTIME_VERSION");
    lua_pushstring(L, cairo_version_string());
    lua_rawset(L, -3);
    add_funcs_to_table(L, constructor_funcs);

    /* Add boolean values to allow Lua code to find out what features are
     * supported in this build. */
    lua_pushliteral(L, "HAS_PDF_SURFACE");
#ifdef CAIRO_HAS_PDF_SURFACE
    lua_pushboolean(L, 1);
#else
    lua_pushboolean(L, 0);
#endif
    lua_rawset(L, -3);
    lua_pushliteral(L, "HAS_PNG_FUNCTIONS");
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    lua_pushboolean(L, 1);
#else
    lua_pushboolean(L, 0);
#endif
    lua_rawset(L, -3);
    lua_pushliteral(L, "HAS_PS_SURFACE");
#ifdef CAIRO_HAS_PS_SURFACE
    lua_pushboolean(L, 1);
#else
    lua_pushboolean(L, 0);
#endif
    lua_rawset(L, -3);
    lua_pushliteral(L, "HAS_SVG_SURFACE");
#ifdef CAIRO_HAS_SVG_SURFACE
    lua_pushboolean(L, 1);
#else
    lua_pushboolean(L, 0);
#endif
    lua_rawset(L, -3);
    lua_pushliteral(L, "HAS_USER_FONT");
#ifdef CAIRO_HAS_USER_FONT
    lua_pushboolean(L, 1);
#else
    lua_pushboolean(L, 0);
#endif
    lua_rawset(L, -3);
    lua_pushliteral(L, "HAS_RECORDING_SURFACE");
#ifdef CAIRO_HAS_RECORDING_SURFACE
    lua_pushboolean(L, 1);
#else
    lua_pushboolean(L, 0);
#endif
    lua_rawset(L, -3);
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
#define ADD_MIME_TYPE(TYPE) \
    lua_pushliteral(L, #TYPE); \
    lua_pushstring(L, CAIRO_ ## TYPE); \
    lua_rawset(L, -3);
    ADD_MIME_TYPE(MIME_TYPE_JP2);
    ADD_MIME_TYPE(MIME_TYPE_JPEG);
    ADD_MIME_TYPE(MIME_TYPE_PNG);
    ADD_MIME_TYPE(MIME_TYPE_URI);
#endif

    /* This is needed to test the byte order which Cairo will use for storing
    * pixel components. */
    lua_pushliteral(L, "BYTE_ORDER");
    lua_pushlstring(L, IS_BIG_ENDIAN ? "argb" : "bgra", 4);
    lua_rawset(L, -3);

    /* Create the metatables for objects of different types. */
    create_object_metatable(L, OOCAIRO_MT_NAME_CONTEXT, "cairo context object",
                            context_methods);
    create_object_metatable(L, OOCAIRO_MT_NAME_FONTFACE, "cairo font face object",
                            fontface_methods);
    create_object_metatable(L, OOCAIRO_MT_NAME_SCALEDFONT, "cairo scaled font object",
                            scaledfont_methods);
    create_object_metatable(L, OOCAIRO_MT_NAME_FONTOPT, "cairo font options object",
                            fontopt_methods);
    create_object_metatable(L, OOCAIRO_MT_NAME_MATRIX, "cairo matrix object",
                            cairmat_methods);
    create_object_metatable(L, OOCAIRO_MT_NAME_PATH, "cairo path object",
                            path_methods);
    create_object_metatable(L, OOCAIRO_MT_NAME_PATTERN, "cairo pattern object",
                            pattern_methods);
    create_object_metatable(L, OOCAIRO_MT_NAME_SURFACE, "cairo surface object",
                            surface_methods);
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
    create_object_metatable(L, OOCAIRO_MT_NAME_REGION, "cairo region object",
                            region_methods);
#endif

    return 1;
}

/* vi:set ts=4 sw=4 expandtab: */
