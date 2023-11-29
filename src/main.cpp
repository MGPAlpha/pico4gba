#include <gba_video.h>
#include "label.hpp"
#include "code.lua.hpp"
#include "cartdata.hpp"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "print.h"

#include "picographics.hpp"

int main() {

	mgba_open();

	cartridge.findSection("lua");

	char buff[256];
	int error;
	lua_State *L = luaL_newstate();   /* opens Lua */
	
	mgba_printf("%x", L);
	mgba_printf("Lib opener address: %x", luaopen_base);

	luaL_openlibs(L);

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

	drawPicoScreen((PICOSCREEN_LINE*)label.data, 56, 10);

	// MODE3_FB[0][0] = 0b0111111111111111;
	// MODE3_FB[10][0] = 0b0000001111111111;
	// MODE3_FB[10][10] = 0b0111110000011111;
	// MODE3_FB[0][10] = 0b0111111111100000;

	while(1);

}