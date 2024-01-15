#include "picolib.hpp"
#include "lauxlib.h"
#include <gba_video.h>
#include <gba_input.h>
#include <gba_dma.h>
#include <fpsqrt.h>
#include "picographics.hpp"

static unsigned long int next = 1;

inline int rand(void) // RAND_MAX assumed to be 32767
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed)
{
    next = seed;
}


int pico_rectFill(lua_State *L) {
	int x0 = lua_tonumber(L, 1);
	int y0 = lua_tonumber(L, 2);
	int x1 = lua_tonumber(L, 3);
	int y1 = lua_tonumber(L, 4);
	int color = lua_tonumber(L, 5);
	for (int i = y0; i <= y1; i++) {
		for (int j = x0; j <= x1; j++) {
			short initial = MODE4_FB[i][j/2];
			if (j%2 == 0) initial = (initial & 0xff00) | color;
			else initial = (initial & 0x00ff) | (color << 8);
			MODE4_FB[i][j/2] = initial;
		}
	}
	return 0;
}

int pico_pSet(lua_State *L) {
	int x = lua_tonumber(L, 1);
	int y = lua_tonumber(L, 2);
	int color = lua_tonumber(L, 3);

	short initial = MODE4_FB[y][x/2];
	if (x%2 == 0) initial = (initial & 0xff00) | color;
	else initial = (initial & 0x00ff) | (color << 8);
	MODE4_FB[y][x/2] = initial;
	return 0;
}

int pico_pGet(lua_State *L) {
	int x = lua_tonumber(L, 1);
	int y = lua_tonumber(L, 2);
	
	short initial = MODE4_FB[y][x/2];
	int res;
	if (x%2 == 0) res = (initial & 0x000f);
	else res = (initial & 0x0f00) >> 8;
	lua_pushinteger(L, res);
	return 1;
}

int pico_btn(lua_State *L) {
	int btn = lua_tonumber(L, 1);

	unsigned short btnmask = 0;
	switch (btn) {
		case 0: {
			btnmask = KEY_LEFT;
		} break;
		case 1: {
			btnmask = KEY_RIGHT;
		} break;
		case 2: {
			btnmask = KEY_UP;
		} break;
		case 3: {
			btnmask = KEY_DOWN;
		} break;
	}

	lua_pushboolean(L, ~REG_KEYINPUT & btnmask);

	return 1;
}

int pico_rnd(lua_State *L) {
	int max = lua_tonumber(L, 1);

	int result = rand() % max;

	lua_pushnumber(L, static_cast<int32_t>(result));

	return 1;
}

int pico_cls(lua_State *L) {
	// mgba_printf("Clearing screen");
	volatile int color = 0;
	DMA3COPY(&color, MODE4_FB, DMA_ENABLE | DMA_DST_INC | DMA32 | DMA_SRC_FIXED | 240*160/4);
	// mgba_printf("screen cleared");

	return 0;
}

int vblankCounter = 0;

int pico_time(lua_State *L) {
	z8::fix32 time = z8::fix32(static_cast<int32_t>(vblankCounter)) / 60;
	lua_pushnumber(L, time);
	return 1;
}

z8::fix32 cosTable[360];

void buildCosTable() {
	for (int i = 0; i < 360; i++) {
		double x = i * (2 * M_PI) / 180;
		cosTable[i] = std::cos(x);
	}
}

int pico_cos(lua_State *L) {

	z8::fix32 ix = lua_tointeger(L, 1);

	int x = (int)ix;

	// lua_pushnumber(L, bios_cos(ix));

	if (x < 0) x = -x;
	
	lua_pushnumber(L, cosTable[x % 360]);

	return 1;

}

int pico_sqrt(lua_State *L) {
	z8::fix32 x = lua_tonumber(L, 1);

	lua_pushnumber(L, sqrt_fx16_16_to_fx16_16(x));

	return 1;
	
}

static const luaL_Reg picolib[] = {
  {"rectfill",   pico_rectFill},
  {"pset",   pico_pSet},
  {"pget",   pico_pGet},
  {"btn",  pico_btn},
  {"rnd",   pico_rnd},
  {"cls",   pico_cls},
  {"time",   pico_time},
  {"t", pico_time},
  {"cos",  pico_cos},
  {"sqrt",   pico_sqrt},
  {NULL, NULL}
};

LUAMOD_API int luaopen_picolib (lua_State *L) {
  lua_pushglobaltable(L);
  luaL_setfuncs(L, picolib, 0);

  buildCosTable();

  return 1;
}