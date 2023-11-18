#ifndef PICOGRAPHICS_HPP
#define PICOGRAPHICS_HPP

#include <cstdint>

typedef uint16_t MODE4_LINE[120];
typedef uint8_t PICOSCREEN_LINE[64];

#define MODE4_FB ((MODE4_LINE *)0x06000000)
#define MODE4_BB ((MODE4_LINE *)0x0600A000)


void drawPicoScreen(PICOSCREEN_LINE* data, uint16_t x, uint16_t y);

#endif