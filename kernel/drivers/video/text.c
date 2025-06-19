#include <stdint.h>
#include <stddef.h>
#include "fb.h"
#include "font_iso.h"

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

void draw_char(uint32_t x, uint32_t y, uint32_t fb_fg_color, uint32_t fb_bg_color, char c) {
	uint8_t *glyph = &iso_font[c * 16];; // pointer to the 16 rows of the char

	for (size_t row = 0; row < FONT_HEIGHT; ++row) {
		uint8_t bits = glyph[row];

		for (size_t col = 0; col < FONT_WIDTH; ++col) {
			if (bits & (1 << col)) { // rightmost bit is first pixel
				put_pixel(x + col, y + row, fb_fg_color);
			} else {
			  put_pixel(x + col, y + row, fb_bg_color);
			}
		}
	}
}

void draw_string(uint32_t x, uint32_t y, uint32_t fb_fg_color, uint32_t fb_bg_color, const char *str) {
	uint32_t cursor_x = x;

	while (*str) {
		if (*str == '\n') {
			y += FONT_HEIGHT;
			cursor_x = x;
		} else {
			draw_char(cursor_x, y, fb_fg_color, fb_bg_color, *str);
			cursor_x += FONT_WIDTH;
		}
		str++;
	}
}