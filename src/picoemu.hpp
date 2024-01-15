#ifndef PICOEMU_HPP
#define PICOEMU_HPP

#include <lua.h>

#include "cartridge.hpp"

class PicoEmulator {
    public:
        PicoEmulator();
        ~PicoEmulator();
        void loadCartridge(const CartSection c);
        static PicoEmulator* getInstance();
        lua_State* getLua();
        unsigned char* getMemory();
        unsigned char memory[0x10000];
    private:
        lua_State *luaInstance;
        static PicoEmulator* instance;
};

#endif