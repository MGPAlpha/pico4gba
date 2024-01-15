#include "picoemu.hpp"
#include <lauxlib.h>
#include <lualib.h>
#include <lua.h>
#include "picolib.hpp"
#include "print.h"

PicoEmulator* PicoEmulator::instance;

PicoEmulator::PicoEmulator() {
    this->luaInstance = luaL_newstate();
    luaL_openlibs(this->luaInstance);
    luaopen_picolib(this->luaInstance);

    for (int i = 0; i < 0x10000; i++) {
        this->memory[i] = 0;
    }
    lua_setpico8memory(this->luaInstance, this->memory);
    PicoEmulator::instance = this;
}

PicoEmulator* PicoEmulator::getInstance() {
    return PicoEmulator::instance;
}


lua_State* PicoEmulator::getLua() {
    return this->luaInstance;
}

unsigned char* PicoEmulator::getMemory() {
    return this->memory;
}

void PicoEmulator::loadCartridge(const CartSection c) {
    ProcessedCartridge cart = ProcessedCartridge(c);

    lua_State *L = this->luaInstance;


    AbstractPicoData luaCode = cart.getLua();
	int error = luaL_loadbuffer(L, luaCode.data, luaCode.size-1, "user_code");

    if (error) {
		mgba_printf("Failed to load user Lua code", lua_tostring(L, -1));
		mgba_printf("%s", lua_tostring(L, -1));
        lua_pop(L, 1);  /* pop error message from the stack */
	}

    cart.deleteSections();
}

