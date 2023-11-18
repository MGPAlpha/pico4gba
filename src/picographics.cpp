#include "picographics.hpp"

void drawPicoScreen(PICOSCREEN_LINE* data, uint16_t x, uint16_t y) {
    for (int j = 0; j < 128; j++) {
		for (int i = 0; i < 64; i++) {
			char pixelPair = data[j][i];
			uint16_t right = (pixelPair & 0xf0);
			uint16_t left = (pixelPair & 0x0f);
			uint16_t fin = (left << 8) | (right >> 4);
			MODE4_FB[j+y][i+x/2] = fin;
		}
	}
}
