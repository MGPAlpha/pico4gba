ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

CARTRIDGE_NAME = drippy

SRC_DIR := src
GEN_SRC_DIR := $(SRC_DIR)/generated
OBJ_DIR := obj
BIN_DIR := bin
LIB_DIR := lib

LIBLUA_A := $(LIB_DIR)/z8luaARM/liblua.a

ROM_O := $(OBJ_DIR)/main.o $(OBJ_DIR)/print.o $(OBJ_DIR)/picographics.o $(OBJ_DIR)/cartridge.o $(OBJ_DIR)/$(CARTRIDGE_NAME).o $(OBJ_DIR)/cartdata.o
ifneq ("$(wildcard $(SRC_DIR)/label.cpp)","")
    ROM_O += $(OBJ_DIR)/label.o
endif
ifneq ("$(wildcard $(SRC_DIR)/code.lua.cpp)","")
    ROM_O += $(OBJ_DIR)/code.lua.o
endif

PICO4GBA_A := $(BIN_DIR)/pico4gba.a
# CLI_O := $(OBJ_DIR)/cli.o

LIB := $(LIBLUA_A)
# EXE := $(BIN_DIR)/pico2gba
# CLI := $(BIN_DIR)/pico2gbacli
# CORE := $(BIN_DIR)/libpico2gba.a
CSRC := $(wildcard $(SRC_DIR)/*.c)
CPPSRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(CPPSRC:.cpp=.o) $(CSRC:.c=.o)

ELF_NAME           = $(BIN_DIR)/$(CARTRIDGE_NAME).elf
ROM_NAME           = $(BIN_DIR)/$(CARTRIDGE_NAME).gba
# BIN_NAME           = $(PRODUCT_NAME)

#MODEL              = -mthumb-interwork -mthumb
MODEL              = -mthumb-interwork -marm -mlong-calls #This makes interrupts work
SPECS              = -specs=gba.specs

# --- Archiver
AS                 = $(DEVKITARM)/bin/arm-none-eabi-as
ASFLAGS            = -mthumb-interwork

# --- Compiler
CC                 = $(DEVKITARM)/bin/arm-none-eabi-g++
CFLAGS             = $(MODEL) -O3 -Wall -pedantic -Wextra -D_ROM=$(ROM_NAME) -I$(DEVKITPRO)/libgba/include -Ilib/z8luaARM

# # --- C++ Compiler
# CPP                = $(DEVKITARM)/bin/arm-none-eabi-g++
# CPPFLAGS           = $(MODEL) -O3 -Wall -pedantic -Wextra -D_ROM=$(ROM_NAME)
# --- Linker
LD                 = $(DEVKITARM)/bin/arm-none-eabi-g++
LDFLAGS            = $(SPECS) $(MODEL) -lm -lstdc++ -L$(DEVKITPRO)/libgba/lib -lgba
# --- Object/Executable Packager
OBJCOPY            = $(DEVKITARM)/bin/arm-none-eabi-objcopy
OBJCOPYFLAGS       = -O binary

AR                 = $(DEVKITARM)/bin/arm-none-eabi-ar rcu

RANLIB             = $(DEVKITARM)/bin/arm-none-eabi-ranlib

# --- ROM Fixer
GBAFIX             = gbafix

.PHONY: all clean

all: $(ROM_NAME)

# --- Build .elf file into .gba ROM file
$(ROM_NAME) : $(ELF_NAME)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(ELF_NAME) $(ROM_NAME)
	$(GBAFIX) $(ROM_NAME)

# --- Build .o files into .elf file
$(ELF_NAME) : $(ROM_O) $(LIB) | $(BIN_DIR)
	$(LD) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
		$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
		$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(GEN_SRC_DIR)/%.c | $(OBJ_DIR)
		$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(GEN_SRC_DIR)/%.cpp | $(OBJ_DIR)
		$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(GEN_SRC_DIR)/$(CARTRIDGE_NAME).c: $(CARTRIDGE_NAME).p8 | $(GEN_SRC_DIR)
		xxd -i $? $@
		sed -i 's/unsigned/const/g' $@
		sed -i '1s/^/#include "..\/cartdata.hpp"\n\n/' $@

$(BIN_DIR) $(OBJ_DIR) $(GEN_SRC_DIR):
		mkdir -p $@
		
clean:
		@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) $(GEN_SRC_DIR)

deep-clean: clean
		@cd $(LIB_DIR)/luaARM && make clean
		@cd $(LIB_DIR)/z8luaARM && make clean

$(LIBLUA_A):
		cd $(LIB_DIR)/z8luaARM && make liblua.a

-include $(OBJ:.o=.d)
