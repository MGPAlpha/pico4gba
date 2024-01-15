#ifndef PICOLIB_HPP
#define PICOLIB_HPP

#include <lua.h>

LUAMOD_API int luaopen_picolib (lua_State *L);

extern int vblankCounter;

void buildCosTable();

int pico_rectFill(lua_State *L);
int pico_pSet(lua_State *L);
int pico_pGet(lua_State *L);
int pico_btn(lua_State *L);
int pico_rnd(lua_State *L);
int pico_cls(lua_State *L);
int pico_time(lua_State *L);
int pico_cos(lua_State *L);

#endif