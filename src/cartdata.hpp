#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "picodata.hpp"

#define CARTDATA drippy_p8
#define CARTLEN drippy_p8_len

extern const char CARTDATA[];
extern const int CARTLEN;

extern const AbstractPicoData cartridge;

#endif