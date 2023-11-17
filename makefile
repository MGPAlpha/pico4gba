ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

PRODUCT_NAME = picocart

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
LIB_DIR := lib

LIBLUA_A := $(LIB_DIR)/z8luaARM/liblua.a

PICO4GBA_A := $(BIN_DIR)/pico4gba.a
CORE_O := $(OBJ_DIR)/main.o
# CLI_O := $(OBJ_DIR)/cli.o

LIB := $(LIBLUA_A)
# EXE := $(BIN_DIR)/pico2gba
# CLI := $(BIN_DIR)/pico2gbacli
# CORE := $(BIN_DIR)/libpico2gba.a
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

ELF_NAME           = $(BIN_DIR)/$(PRODUCT_NAME).elf
ROM_NAME           = $(BIN_DIR)/$(PRODUCT_NAME).gba
# BIN_NAME           = $(PRODUCT_NAME)

#MODEL              = -mthumb-interwork -mthumb
MODEL              = -mthumb-interwork -marm -mlong-calls #This makes interrupts work
SPECS              = -specs=gba.specs

# --- Archiver
AS                 = $(DEVKITARM)/bin/arm-none-eabi-as
ASFLAGS            = -mthumb-interwork

# --- Compiler
CC                 = $(DEVKITARM)/bin/arm-none-eabi-gcc
CFLAGS             = $(MODEL) -O3 -Wall -pedantic -Wextra -std=c99 -D_ROM=$(ROM_NAME) -I$(DEVKITPRO)/libgba/include

# --- C++ Compiler
CPP                = $(DEVKITARM)/bin/arm-none-eabi-g++
CPPFLAGS           = $(MODEL) -O3 -Wall -pedantic -Wextra -D_ROM=$(ROM_NAME)
# --- Linker
LD                 = $(DEVKITARM)/bin/arm-none-eabi-gcc
LDFLAGS            = $(SPECS) $(MODEL) -lm -lstdc++ -L$(DEVKITPRO)/libgba/lib -lgba
# --- Object/Executable Packager
OBJCOPY            = $(DEVKITARM)/bin/arm-none-eabi-objcopy
OBJCOPYFLAGS       = -O binary

# --- ROM Fixer
GBAFIX             = gbafix

.PHONY: all clean

all: $(ROM_NAME)

# --- Build .elf file into .gba ROM file
$(ROM_NAME) : $(ELF_NAME)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(ELF_NAME) $(ROM_NAME)
	$(GBAFIX) $(ROM_NAME)

# --- Build .o files into .elf file
$(ELF_NAME) : $(PICO4GBA_A)
	$(LD) $^ $(LDFLAGS) -o $@

$(PICO4GBA_A): $(CORE_O) $(LIB) | $(BIN_DIR)
		$(AR) -rc $@ $?

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
		$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
		mkdir -p $@
		
clean:
		@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

deep-clean: clean
		@cd $(LIB_DIR)/luaARM && make clean
		@cd $(LIB_DIR)/z8luaARM && make clean

$(LIBLUA_A):
		cd $(LIB_DIR)/z8luaARM && make liblua.a

-include $(OBJ:.o=.d)
