#ifndef INC_LUA_OOCAIRO_H
#define INC_LUA_OOCAIRO_H

#include <lua.h>
#include <lauxlib.h>
#include <cairo.h>

#define MT_NAME_CONTEXT    ("6404c570-6711-11dd-b66f-00e081225ce5")
#define MT_NAME_FONTFACE   ("ee272774-6a1e-11dd-86de-00e081225ce5")
#define MT_NAME_FONTOPT    ("8ae95550-9887-11dd-922a-00e081225ce5")
#define MT_NAME_MATRIX     ("6e2f4c64-6711-11dd-acfc-00e081225ce5")
#define MT_NAME_PATH       ("6d83bf34-6711-11dd-b4c2-00e081225ce5")
#define MT_NAME_PATTERN    ("6dd49a26-6711-11dd-88fd-00e081225ce5")
#define MT_NAME_SCALEDFONT ("b8012f94-98b0-11dd-b174-00e081225ce5")
#define MT_NAME_SURFACE    ("6d31a064-6711-11dd-bdd8-00e081225ce5")

int luaopen_oocairo (lua_State *L);

int oocairo_surface_push (lua_State *L, cairo_surface_t *surface);

#endif  /* INC_LUA_OOCAIRO_H */
/* vi:set ts=4 sw=4 expandtab: */
