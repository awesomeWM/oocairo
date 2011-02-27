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

#ifndef INC_LUA_OOCAIRO_H
#define INC_LUA_OOCAIRO_H

#include <lua.h>
#include <lauxlib.h>
#include <cairo.h>

#define OOCAIRO_MT_NAME_CONTEXT    ("6404c570-6711-11dd-b66f-00e081225ce5")
#define OOCAIRO_MT_NAME_FONTFACE   ("ee272774-6a1e-11dd-86de-00e081225ce5")
#define OOCAIRO_MT_NAME_FONTOPT    ("8ae95550-9887-11dd-922a-00e081225ce5")
#define OOCAIRO_MT_NAME_MATRIX     ("6e2f4c64-6711-11dd-acfc-00e081225ce5")
#define OOCAIRO_MT_NAME_PATH       ("6d83bf34-6711-11dd-b4c2-00e081225ce5")
#define OOCAIRO_MT_NAME_PATTERN    ("6dd49a26-6711-11dd-88fd-00e081225ce5")
#define OOCAIRO_MT_NAME_SCALEDFONT ("b8012f94-98b0-11dd-b174-00e081225ce5")
#define OOCAIRO_MT_NAME_SURFACE    ("6d31a064-6711-11dd-bdd8-00e081225ce5")

int luaopen_oocairo (lua_State *L);

int oocairo_surface_push (lua_State *L, cairo_surface_t *surface);

#endif  /* INC_LUA_OOCAIRO_H */
/* vi:set ts=4 sw=4 expandtab: */
