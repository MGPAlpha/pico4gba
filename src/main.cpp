#include <gba_video.h>
#include <gba_input.h>
#include "cartdata.hpp"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <gba_dma.h>
#include <fpsqrt.h>

#include <gba_affine.h>
// #include <cstdlib>

#include "print.h"

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


void buildCosTable();


int pico_rectFill(lua_State *L);
int pico_pSet(lua_State *L);
int pico_pGet(lua_State *L);
int pico_btn(lua_State *L);
int pico_rnd(lua_State *L);
int pico_cls(lua_State *L);
int pico_time(lua_State *L);
int pico_cos(lua_State *L);

int pico_rectFill(lua_State *L) {
	int x0 = lua_tonumber(L, 1);
	int y0 = lua_tonumber(L, 2);
	int x1 = lua_tonumber(L, 3);
	int y1 = lua_tonumber(L, 4);
	int color = lua_tonumber(L, 5);
	mgba_printf("Rectfill: %x, %x, %x, %x, %x", x0, y0, x1, y1, color);
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

static int vblankCounter = 0;

int pico_time(lua_State *L) {
	z8::fix32 time = z8::fix32(static_cast<int32_t>(vblankCounter)) / 60;
	mgba_printf("Time: %x", time);
	lua_pushnumber(L, time);
	return 1;
}

static t_ObjAffineSource affineTrigSource = {0x0100, 0x5000, 0};
static t_ObjAffineDest affineTrigDest = {0x0100, 0, 0, 0x0100};	

z8::fix32 cosTable[360];

void buildCosTable() {
	for (int i = 0; i < 360; i++) {
		double x = i * (2 * M_PI) / 180;
		cosTable[i] = std::cos(x);
	}
}

int pico_cos(lua_State *L) {

	z8::fix32 ix = lua_tonumber(L, 1);

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

int main() {

	mgba_open();

	ProcessedCartridge cart = ProcessedCartridge(cartridge);
	AbstractPicoData label = cart.getLabel();
	AbstractPicoData lua = cart.getLua();

	REG_DISPCNT = MODE_4 | BG2_ENABLE;

	BG_PALETTE[0] = RGB8(0, 0, 0);
	BG_PALETTE[1] = RGB8(29, 43, 83);
	BG_PALETTE[2] = RGB8(126, 37, 83);
	BG_PALETTE[3] = RGB8(0, 135, 81);
	BG_PALETTE[4] = RGB8(171, 82, 54);
	BG_PALETTE[5] = RGB8(95, 87, 79);
	BG_PALETTE[6] = RGB8(194, 195, 199);
	BG_PALETTE[7] = RGB8(255, 241, 232);
	BG_PALETTE[8] = RGB8(255, 0, 77);
	BG_PALETTE[9] = RGB8(255, 163, 0);
	BG_PALETTE[10] = RGB8(255, 236, 39);
	BG_PALETTE[11] = RGB8(0, 228, 54);
	BG_PALETTE[12] = RGB8(41, 173, 255);
	BG_PALETTE[13] = RGB8(131, 118, 156);
	BG_PALETTE[14] = RGB8(255, 119, 168);
	BG_PALETTE[15] = RGB8(255, 204, 170);

	drawPicoScreen((PICOSCREEN_LINE*)label.data, 0, 0);


	char buff[256];

	int error;
	lua_State *L = luaL_newstate();   /* opens Lua */
	
	mgba_printf("%x", L);
	mgba_printf("Lib opener address: %x", luaopen_base);

	luaL_openlibs(L);
	lua_pushcfunction(L, pico_rectFill);
	lua_setglobal(L, "rectfill");
	lua_pushcfunction(L, pico_pSet);
	lua_setglobal(L, "pset");
	lua_pushcfunction(L, pico_pGet);
	lua_setglobal(L, "pget");
	lua_pushcfunction(L, pico_btn);
	lua_setglobal(L, "btn");
	lua_pushcfunction(L, pico_rnd);
	lua_setglobal(L, "rnd");
	lua_pushcfunction(L, pico_cls);
	lua_setglobal(L, "cls");
	lua_pushcfunction(L, pico_time);
	lua_setglobal(L, "time");
	lua_pushcfunction(L, pico_time);
	lua_setglobal(L, "t");
	lua_pushnil(L);
	lua_setglobal(L, "cos");
	lua_pushcfunction(L, pico_cos);
	lua_setglobal(L, "cos");

	lua_pushnil(L);
	lua_setglobal(L, "sqrt");
	lua_pushcfunction(L, pico_sqrt);
	lua_setglobal(L, "sqrt");

	buildCosTable();

	error = luaL_loadbuffer(L, lua.data, lua.size-1, "user_code");

	if (error) {
		mgba_printf("%s", lua_tostring(L, -1));
          lua_pop(L, 1);  /* pop error message from the stack */
	}

	error = lua_pcall(L, 0, 0, 0);

	if (error) {
		mgba_printf("%s", lua_tostring(L, -1));
          lua_pop(L, 1);  /* pop error message from the stack */
	}



	// MODE3_FB[0][0] = 0b0111111111111111;
	// MODE3_FB[10][0] = 0b0000001111111111;
	// MODE3_FB[10][10] = 0b0111110000011111;
	// MODE3_FB[0][10] = 0b0111111111100000;

	while(1) {
		// lua_getglobal(L, "_update");
		// error = lua_pcall(L, 0, 0, 0);
		// if (error) {
		// 	mgba_printf("%s", lua_tostring(L, -1));
        //   lua_pop(L, 1);  /* pop error message from the stack */
		// }

		while (REG_VCOUNT >= 160);
		while (REG_VCOUNT < 160);
		vblankCounter++;

		lua_getglobal(L, "_draw");
		error = lua_pcall(L, 0, 0, 0);
		if (error) {
			mgba_printf("%s", lua_tostring(L, -1));
          lua_pop(L, 1);  /* pop error message from the stack */
		}
	}

}