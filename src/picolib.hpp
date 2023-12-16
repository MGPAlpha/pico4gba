#ifndef PICOLIB_HPP
#define PICOLIB_HPP

#include <lua.h>

LUAMOD_API int luaopen_picolib (lua_State *L);

extern int vblankCounter;

#endif