#include <stdint.h>
#include "fb.h"

void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t thickness, uint32_t color_tl, uint32_t color_br) {
	for (uint32_t t = 0; t < thickness; ++t) {
		// top
		for (uint32_t ix = x + t; ix < x + width - t; ++ix) {
			put_pixel(ix, y + t, color_tl);
		}
		// left
		for (uint32_t iy = y + t; iy < y + height - t; ++iy) {
			put_pixel(x + t, iy, color_tl);
		}
	}

	for (uint32_t t = 0; t < thickness; ++t) {
		// bottom
		for (uint32_t ix = x + t; ix < x + width - t; ++ix) {
			put_pixel(ix, y + height - 1 - t, color_br);
		}
		// right
		for (uint32_t iy = y + t; iy < y + height - t; ++iy) {
			put_pixel(x + width - 1 - t, iy, color_br);
		}
	}
}

void draw_filled_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
	for (uint32_t iy = y; iy < y+height; ++iy) {
		for (uint32_t ix = x; ix < x+width; ++ix) {
			put_pixel(ix, iy, color);
		}
	}
}