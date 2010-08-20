#ifndef INC_LUA_OOCAIRO_H
#define INC_LUA_OOCAIRO_H

#include <lua.h>
#include <lauxlib.h>
#include <cairo.h>

int luaopen_oocairo (lua_State *L);

int oocairo_surface_push (lua_State *L, cairo_surface_t *surface);

#endif  /* INC_LUA_OOCAIRO_H */
/* vi:set ts=4 sw=4 expandtab: */
