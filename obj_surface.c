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
image_surface_create (lua_State *L) {
    cairo_format_t fmt;
    int width, height;
    SurfaceUserdata *surface;

    fmt = format_from_lua(L, 1);
    width = luaL_checkint(L, 2);
    luaL_argcheck(L, width >= 0, 2, "image width cannot be negative");
    height = luaL_checkint(L, 3);
    luaL_argcheck(L, height >= 0, 3, "image height cannot be negative");

    surface = create_surface_userdata(L);
    surface->surface = cairo_image_surface_create(fmt, width, height);
    return 1;
}

static int
image_surface_create_from_data (lua_State *L) {
    cairo_format_t fmt;
    int width, height, stride, min_stride;
    const char *data;
    size_t data_len;
    SurfaceUserdata *surface;

    data = luaL_checklstring(L, 1, &data_len);
    fmt = format_from_lua(L, 2);
    width = luaL_checkint(L, 3);
    luaL_argcheck(L, width >= 0, 3, "image width cannot be negative");
    height = luaL_checkint(L, 4);
    luaL_argcheck(L, height >= 0, 4, "image height cannot be negative");
    stride = luaL_checkint(L, 5);

    /* Check that the stride is big enough for one row of pixels. */
    if (fmt == CAIRO_FORMAT_ARGB32 || fmt == CAIRO_FORMAT_RGB24)
        min_stride = width * 4;
    else if (fmt == CAIRO_FORMAT_A1)
        min_stride = (width + 31) / 32;
    else
        min_stride = width;
    luaL_argcheck(L, stride >= min_stride, 5,
                  "stride value too small for this width and pixel format");

    luaL_argcheck(L, data_len >= (size_t) stride * height, 1,
                  "image data string not long enough for this image size");

    surface = create_surface_userdata(L);
    surface->image_buffer = malloc(data_len);
    assert(surface->image_buffer);
    memcpy(surface->image_buffer, data, data_len);
    surface->surface = cairo_image_surface_create_for_data(
                            surface->image_buffer, fmt, width, height, stride);
    return 1;
}

struct ReadInfoLuaStream {
    lua_State *L;
    int fhpos;
    const char *errmsg;
};

static cairo_status_t
read_chunk_from_fh (void *closure, unsigned char *buf, unsigned int lentoread)
{
    struct ReadInfoLuaStream *info = closure;
    const char *s;
    size_t len;

    lua_State *L = info->L;
    lua_getfield(L, info->fhpos, "read");
    lua_pushvalue(L, info->fhpos);
    lua_pushnumber(L, lentoread);
    if (lua_pcall(L, 2, 1, 0)) {
        if (lua_isstring(L, -1))
            info->errmsg = lua_tostring(L, -1);
        return CAIRO_STATUS_READ_ERROR;
    }

    s = lua_tolstring(L, -1, &len);
    if (!s) {
        info->errmsg = "'read' method on file handle didn't return string";
        return CAIRO_STATUS_READ_ERROR;
    }
    if (len != lentoread) {
        info->errmsg = "'read' method on file handle returned wrong amount"
                           " of data";
        return CAIRO_STATUS_READ_ERROR;
    }

    memcpy(buf, s, len);
    lua_pop(L, 1);
    return CAIRO_STATUS_SUCCESS;
}

#ifdef CAIRO_HAS_PNG_FUNCTIONS
static int
image_surface_create_from_png (lua_State *L) {
    SurfaceUserdata *surface = create_surface_userdata(L);

    if (lua_isstring(L, 1)) {
        const char *filename = luaL_checkstring(L, 1);
        surface->surface = cairo_image_surface_create_from_png(filename);
        switch (cairo_surface_status(surface->surface)) {
            case CAIRO_STATUS_FILE_NOT_FOUND:
                return luaL_error(L, "PNG file '%s' not found", filename);
            case CAIRO_STATUS_READ_ERROR:
                return luaL_error(L, "error reading PNG file '%s'", filename);
            default:;
        }
    }
    else {
        struct ReadInfoLuaStream info;

        info.L = L;
        info.fhpos = 1;
        info.errmsg = 0;
        surface->surface = cairo_image_surface_create_from_png_stream(
                                    read_chunk_from_fh, &info);
        if (!surface->surface) {
            lua_pushliteral(L, "error reading PNG file from Lua file handle");
            if (info.errmsg) {
                lua_pushliteral(L, ": ");
                lua_pushstring(L, info.errmsg);
                lua_concat(L, 3);
            }
            return lua_error(L);
        }
    }

    return 1;
}
#endif

/* This function is used for several types of surface which all have the
 * same type of construction, and are all a bit complicated because they
 * can write their output to a Lua file handle. */
typedef cairo_surface_t * (*SimpleSizeCreatorFunc) (const char *,
                                                    double, double);
typedef cairo_surface_t * (*StreamSizeCreatorFunc) (cairo_write_func_t, void *,
                                                    double, double);
static void
surface_create_with_size (lua_State *L, SimpleSizeCreatorFunc simplefunc, StreamSizeCreatorFunc streamfunc)
{
    double width, height;
    SurfaceUserdata *surface;
    int filetype;

    width = luaL_checknumber(L, 2);
    height = luaL_checknumber(L, 3);
    luaL_argcheck(L, width >= 0, 2, "image width cannot be negative");
    luaL_argcheck(L, height >= 0, 3, "image height cannot be negative");

    surface = create_surface_userdata(L);

    filetype = lua_type(L, 1);
    if (filetype == LUA_TSTRING || filetype == LUA_TNUMBER) {
        const char *filename = lua_tostring(L, 1);
        surface->surface = simplefunc(filename, width, height);
        if (cairo_surface_status(surface->surface) != CAIRO_STATUS_SUCCESS)
            luaL_error(L, "error creating surface for filename '%s'", filename);
    }
    else if (filetype == LUA_TUSERDATA || filetype == LUA_TTABLE) {
        lua_pushvalue(L, 1);
        surface->fhref = luaL_ref(L, LUA_REGISTRYINDEX);

        surface->surface = streamfunc(write_chunk_to_fh, surface,
                                      width, height);
        if (cairo_surface_status(surface->surface) != CAIRO_STATUS_SUCCESS) {
            lua_pushliteral(L, "error writing surface output file to Lua"
                            " file handle");
            if (surface->errmsg) {
                lua_pushliteral(L, ": ");
                lua_pushstring(L, surface->errmsg);
                lua_concat(L, 3);
            }
            lua_error(L);
        }
    }
    else
        luaL_typerror(L, 1, "filename or file handle object");
}

#ifdef CAIRO_HAS_PDF_SURFACE
static int
pdf_surface_create (lua_State *L) {
    surface_create_with_size(L, cairo_pdf_surface_create,
                             cairo_pdf_surface_create_for_stream);
    return 1;
}
#endif

#ifdef CAIRO_HAS_PS_SURFACE
static int
ps_surface_create (lua_State *L) {
    surface_create_with_size(L, cairo_ps_surface_create,
                             cairo_ps_surface_create_for_stream);
    return 1;
}
#endif

#ifdef CAIRO_HAS_SVG_SURFACE
static int
svg_surface_create (lua_State *L) {
    surface_create_with_size(L, cairo_svg_surface_create,
                             cairo_svg_surface_create_for_stream);
    return 1;
}
#endif

#ifdef CAIRO_HAS_PS_SURFACE
static int
ps_get_levels (lua_State *L) {
    const cairo_ps_level_t *levels;
    int num_levels, i;
    cairo_ps_get_levels(&levels, &num_levels);

    lua_createtable(L, num_levels, 0);
    for (i = 0; i < num_levels; ++i) {
        lua_pushstring(L, cairo_ps_level_to_string(levels[i]));
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}
#endif

#ifdef CAIRO_HAS_RECORDING_SURFACE
static int
recording_surface_create (lua_State *L) {
    cairo_content_t content;
    SurfaceUserdata *surface;
    cairo_rectangle_t extents;
    cairo_rectangle_t *pextents = NULL;

    content = content_from_lua(L, 1);
    if (lua_gettop(L) != 1) {
        extents.x      = luaL_checknumber(L, 2);
        extents.y      = luaL_checknumber(L, 3);
        extents.width  = luaL_checknumber(L, 4);
        extents.height = luaL_checknumber(L, 5);
        pextents = &extents;
        luaL_argcheck(L, extents.width >= 0, 4, "recording surface width cannot be negative");
        luaL_argcheck(L, extents.height >= 0, 5, "recording surface height cannot be negative");
    }

    surface = create_surface_userdata(L);
    surface->surface = cairo_recording_surface_create(content, pextents);
    return 1;
}

static int
recording_surface_ink_extents (lua_State *L) {
    double x0, y0, width, height;
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);

    cairo_recording_surface_ink_extents(*obj, &x0, &y0, &width, &height);
    lua_pushnumber(L, x0);
    lua_pushnumber(L, y0);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    return 4;
}
#endif

#ifdef CAIRO_HAS_SVG_SURFACE
static int
svg_get_versions (lua_State *L) {
    const cairo_svg_version_t *versions;
    int num_versions, i;
    cairo_svg_get_versions(&versions, &num_versions);

    lua_createtable(L, num_versions, 0);
    for (i = 0; i < num_versions; ++i) {
        lua_pushstring(L, cairo_svg_version_to_string(versions[i]));
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}
#endif

static int
surface_create_similar (lua_State *L) {
    cairo_surface_t **oldobj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_content_t content;
    int width, height;
    SurfaceUserdata *surface;

    content = content_from_lua(L, 2);
    width = luaL_checkint(L, 3);
    luaL_argcheck(L, width >= 0, 3, "image width cannot be negative");
    height = luaL_checkint(L, 4);
    luaL_argcheck(L, height >= 0, 4, "image height cannot be negative");

    surface = create_surface_userdata(L);
    surface->surface = cairo_surface_create_similar(*oldobj, content,
                                                    width, height);
    return 1;
}

static int
surface_eq (lua_State *L) {
    cairo_surface_t **obj1 = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_surface_t **obj2 = luaL_checkudata(L, 2, OOCAIRO_MT_NAME_SURFACE);
    lua_pushboolean(L, *obj1 == *obj2);
    return 1;
}

static int
surface_gc (lua_State *L) {
    SurfaceUserdata *ud = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    free_surface_userdata(ud);
    return 0;
}

static int
surface_copy_page (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_surface_copy_page(*obj);
    return 0;
}

static int
surface_finish (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_surface_finish(*obj);
    return 0;
}

static int
surface_flush (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_surface_flush(*obj);
    return 0;
}

static int
surface_get_content (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    return content_to_lua(L, cairo_surface_get_content(*obj));
}

static int
surface_get_data (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    int height = cairo_image_surface_get_height(*obj);
    int stride = cairo_image_surface_get_stride(*obj);
    const char *data = (const char *) cairo_image_surface_get_data(*obj);
    if (!data)
        return 0;   /* not an image surface */
    lua_pushlstring(L, data, height * stride);
    lua_pushnumber(L, stride);
    return 2;
}

static int
surface_get_device_offset (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    double x, y;
    cairo_surface_get_device_offset(*obj, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

#ifdef CAIRO_HAS_PS_SURFACE
static int
surface_get_eps (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    if (cairo_surface_get_type(*obj) != CAIRO_SURFACE_TYPE_PS)
        return luaL_error(L, "method 'get_eps' only works on PostScript"
                          " surfaces");
    lua_pushboolean(L, cairo_ps_surface_get_eps(*obj));
    return 1;
}
#endif

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int
surface_get_fallback_resolution (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    double x, y;
    cairo_surface_get_fallback_resolution(*obj, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}
#endif

static int
surface_get_font_options (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_font_options_t **opt = create_fontopt_userdata(L);
    *opt = cairo_font_options_create();
    cairo_surface_get_font_options(*obj, *opt);
    return 1;
}

static int
surface_get_format (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    if (cairo_surface_get_type(*obj) != CAIRO_SURFACE_TYPE_IMAGE)
        return luaL_error(L, "method 'get_format' only works on image surfaces");
    return format_to_lua(L, cairo_image_surface_get_format(*obj));
}

static int
surface_get_gdk_pixbuf (lua_State *L) {
    cairo_surface_t **surface;
    cairo_format_t format;
    int width, height, stridei, strideo;
    unsigned char *buffer, *rowpo, *po;
    const char *rowpi, *pi;
    size_t buffer_len;
    int x, y;
    int has_alpha;

    surface = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    if (cairo_surface_get_type(*surface) != CAIRO_SURFACE_TYPE_IMAGE)
        return luaL_error(L, "pixbufs can only be made from image surfaces");

    format = cairo_image_surface_get_format(*surface);
    if (format != CAIRO_FORMAT_ARGB32 && format != CAIRO_FORMAT_RGB24)
        return luaL_error(L, "can't make pixbuf from this image format");
    has_alpha = (format == CAIRO_FORMAT_ARGB32);

    width = cairo_image_surface_get_width(*surface);
    height = cairo_image_surface_get_height(*surface);
    stridei = cairo_image_surface_get_stride(*surface);
    if (has_alpha)
        strideo = stridei;  /* might as well keep Cairo stride */
    else
        strideo = ((3 * width) + 7) & ~7;   /* align to 8 bytes */
    buffer_len = strideo * height;
    buffer = malloc(buffer_len);
    assert(buffer);

    rowpi = (const char *) cairo_image_surface_get_data(*surface);
    rowpo = buffer;

    /* Copy pixels from Cairo's pixel format to GdkPixbuf's, which is slightly
     * different. */
    if (IS_BIG_ENDIAN) {
        for (y = 0; y < height; ++y) {
            pi = rowpi;
            po = rowpo;
            for (x = 0; x < width; ++x) {
                *po++ = pi[1];
                *po++ = pi[2];
                *po++ = pi[3];
                if (has_alpha)
                    *po++ = pi[0];
                pi += 4;
            }
            rowpi += stridei;
            rowpo += strideo;
        }
    }
    else {
        for (y = 0; y < height; ++y) {
            pi = rowpi;
            po = rowpo;
            for (x = 0; x < width; ++x) {
                *po++ = pi[2];
                *po++ = pi[1];
                *po++ = pi[0];
                if (has_alpha)
                    *po++ = pi[3];
                pi += 4;
            }
            rowpi += stridei;
            rowpo += strideo;
        }
    }

    /* The buffer needs to be copied in to a Lua string so that it can
     * be passed to Lua-Gnome. */
    lua_pushlstring(L, (const char *) buffer, buffer_len);
    free(buffer);

    /* Use Lua-Gnome function to construct the GdkPixbuf object, so that we
     * don't have to link directly with GDK, and so that the resulting object
     * can be used with the rest of Lua-Gnome. */
    get_gtk_module_function(L, "gdk_pixbuf_new_from_data");
    lua_pushvalue(L, -2);
    get_gtk_module_function(L, "GDK_COLORSPACE_RGB");
    lua_pushboolean(L, has_alpha);
    lua_pushnumber(L, 8);
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    lua_pushnumber(L, strideo);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_call(L, 9, 1);

    /* Keep a reference to the Lua string used to store the data, since
     * it needs to be kept around for as long as the object is in use. */
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "_pixbuf_buffer_string");

    return 1;
}

static int
surface_get_height (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    if (cairo_surface_get_type(*obj) != CAIRO_SURFACE_TYPE_IMAGE)
        return luaL_error(L, "method 'get_height' only works on image surfaces");
    lua_pushnumber(L, cairo_image_surface_get_height(*obj));
    return 1;
}

static int
surface_get_type (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    return surface_type_to_lua(L, cairo_surface_get_type(*obj));
}

static int
surface_get_width (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    if (cairo_surface_get_type(*obj) != CAIRO_SURFACE_TYPE_IMAGE)
        return luaL_error(L, "method 'get_width' only works on image surfaces");
    lua_pushnumber(L, cairo_image_surface_get_width(*obj));
    return 1;
}

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int
surface_has_show_text_glyphs (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    lua_pushboolean(L, cairo_surface_has_show_text_glyphs(*obj));
    return 1;
}
#endif

static int
surface_set_device_offset (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_surface_set_device_offset(*obj, luaL_checknumber(L, 2),
                                    luaL_checknumber(L, 3));
    return 0;
}

#ifdef CAIRO_HAS_PS_SURFACE
static int
surface_set_eps (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    if (cairo_surface_get_type(*obj) != CAIRO_SURFACE_TYPE_PS)
        return luaL_error(L, "method 'set_eps' only works on PostScript"
                          " surfaces");
    cairo_ps_surface_set_eps(*obj, lua_toboolean(L, 2));
    return 0;
}
#endif

static int
surface_set_fallback_resolution (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_surface_set_fallback_resolution(*obj, luaL_checknumber(L, 2),
                                          luaL_checknumber(L, 3));
    return 0;
}

#if defined(CAIRO_HAS_PDF_SURFACE) || defined(CAIRO_HAS_PS_SURFACE)
static int
surface_set_size (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_surface_type_t type = cairo_surface_get_type(*obj);
    double width = luaL_checknumber(L, 2), height = luaL_checknumber(L, 3);
#ifdef CAIRO_HAS_PDF_SURFACE
    if (type == CAIRO_SURFACE_TYPE_PDF)
        cairo_pdf_surface_set_size(*obj, width, height);
#endif
#if defined(CAIRO_HAS_PDF_SURFACE) && defined(CAIRO_HAS_PS_SURFACE)
    else
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    if (type == CAIRO_SURFACE_TYPE_PS)
        cairo_ps_surface_set_size(*obj, width, height);
#endif
    else
        return luaL_error(L, "method 'set_size' only works on PostScript and"
                          " PDF surfaces");
    return 0;
}
#endif

static int
surface_show_page (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_surface_show_page(*obj);
    return 0;
}

#ifdef CAIRO_HAS_PNG_FUNCTIONS
static int
surface_write_to_png (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    int filetype = lua_type(L, 2);

    if (filetype == LUA_TSTRING || filetype == LUA_TNUMBER) {
        const char *filename = lua_tostring(L, 2);
        if (cairo_surface_write_to_png(*obj, filename) != CAIRO_STATUS_SUCCESS)
            return luaL_error(L, "error writing surface to PNG file '%s'",
                              filename);
    }
    else if (filetype == LUA_TUSERDATA || filetype == LUA_TTABLE) {
        SurfaceUserdata info;
        init_surface_userdata(L, &info);
        lua_pushvalue(L, 2);
        info.fhref = luaL_ref(L, LUA_REGISTRYINDEX);

        if (cairo_surface_write_to_png_stream(*obj, write_chunk_to_fh, &info)
                != CAIRO_STATUS_SUCCESS)
        {
            lua_pushliteral(L, "error writing PNG file to Lua file handle");
            if (info.errmsg) {
                lua_pushliteral(L, ": ");
                lua_pushstring(L, info.errmsg);
                lua_concat(L, 3);
            }
            free_surface_userdata(&info);
            return lua_error(L);
        }

        free_surface_userdata(&info);
    }
    else
        return luaL_typerror(L, 1, "filename or file handle object");

    return 0;
}
#endif

#if defined(CAIRO_HAS_PDF_SURFACE) && CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
static int
restrict_to_version (lua_State *L){
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    cairo_pdf_surface_restrict_to_version(*obj, pdf_version_from_lua(L, 2));
    return 0;
}
#endif

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
static int
create_for_rectangle(lua_State *L)
{
    SurfaceUserdata *surface;
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    double width = luaL_checknumber(L, 4);
    double height = luaL_checknumber(L, 5);

    luaL_argcheck(L, width >= 0, 3, "surface width cannot be negative");
    luaL_argcheck(L, height >= 0, 4, "surface height cannot be negative");

    surface = create_surface_userdata(L);
    surface->surface = cairo_surface_create_for_rectangle(*obj, x, y, width, height);
    return 1;
}
#endif

static int
surface_status (lua_State *L) {
    cairo_surface_t **obj = luaL_checkudata(L, 1, OOCAIRO_MT_NAME_SURFACE);
    return push_cairo_status(L, cairo_surface_status(*obj));
}

static const luaL_Reg
surface_methods[] = {
    { "__eq", surface_eq },
    { "__gc", surface_gc },
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
    { "create_for_rectangle", create_for_rectangle },
#endif
    { "copy_page", surface_copy_page },
    { "finish", surface_finish },
    { "flush", surface_flush },
    { "get_content", surface_get_content },
    { "get_data", surface_get_data },
    { "get_device_offset", surface_get_device_offset },
#ifdef CAIRO_HAS_PS_SURFACE
    { "get_eps", surface_get_eps },
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    { "get_fallback_resolution", surface_get_fallback_resolution },
#endif
    { "get_font_options", surface_get_font_options },
    { "get_format", surface_get_format },
    { "get_gdk_pixbuf", surface_get_gdk_pixbuf },
    { "get_height", surface_get_height },
    { "get_type", surface_get_type },
    { "get_width", surface_get_width },
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    { "has_show_text_glyphs", surface_has_show_text_glyphs },
#endif
#if defined(CAIRO_HAS_PDF_SURFACE) && CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
    { "restrict_to_version", restrict_to_version },
#endif
    { "set_device_offset", surface_set_device_offset },
#ifdef CAIRO_HAS_PS_SURFACE
    { "set_eps", surface_set_eps },
#endif
    { "set_fallback_resolution", surface_set_fallback_resolution },
#if defined(CAIRO_HAS_PDF_SURFACE) || defined(CAIRO_HAS_PS_SURFACE)
    { "set_size", surface_set_size },
#endif
    { "show_page", surface_show_page },
    { "status", surface_status },
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    { "write_to_png", surface_write_to_png },
#endif
    { 0, 0 }
};

/* vi:set ts=4 sw=4 expandtab: */
