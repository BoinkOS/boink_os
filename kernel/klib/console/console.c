#include "console.h"
#include "../../drivers/video/fb.h"
#include "../../drivers/video/text.h"
#include "../input/keyboard/keyboard.h"
#include "../uptime/uptime.h"
#include "../../utils.h"

static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;
static uint32_t text_color = 0xFFFFFF;
static uint32_t background_color = 0x000000;

static uint32_t max_cols = 0;
static uint32_t max_rows = 0;

static uint8_t cursor_enabled = 1;
static uint8_t cursor_drawn = 1;
static uint32_t last_blink_tick = 0;
static uint32_t blink_interval_ticks = 50;

void console_init() {
	max_cols = fb_width / FONT_WIDTH;
	max_rows = fb_height / FONT_HEIGHT;
	console_clear();
}

static void console_scroll() {
	uint32_t row_bytes = fb_pitch * FONT_HEIGHT;
	uint8_t *fb = (uint8_t *)framebuffer;

	for (uint32_t y = 0; y < (fb_height - FONT_HEIGHT); ++y) {
		for (uint32_t x = 0; x < fb_pitch; ++x) {
			fb[y * fb_pitch + x] = fb[(y + FONT_HEIGHT) * fb_pitch + x];
		}
	}

	// clear the last row
	for (uint32_t y = fb_height - FONT_HEIGHT; y < fb_height; ++y) {
		for (uint32_t x = 0; x < fb_width; ++x) {
			put_pixel(x, y, background_color);
		}
	}

	if (cursor_y > 0) {
		cursor_y--;
	}
}


void console_putc(char c) {
	if (cursor_drawn) {
		erase_cursor();
		cursor_drawn = 0;
	}

	if (c == '\n') {
		cursor_x = 0;
		cursor_y++;
		if (cursor_y >= max_rows) {
			console_scroll();
			cursor_y = max_rows - 1;
		}
	} else if (c == '\b') {
		if (cursor_x > 0) cursor_x--;
		draw_char(cursor_x * FONT_WIDTH, cursor_y * FONT_HEIGHT, text_color, background_color, ' ');
	} else {
		draw_char(cursor_x * FONT_WIDTH, cursor_y * FONT_HEIGHT, text_color, background_color, c);
		cursor_x++;
		if (cursor_x >= max_cols) {
			cursor_x = 0;
			cursor_y++;
			if (cursor_y >= max_rows) {
				console_scroll();
				cursor_y = max_rows - 1;
			}
		}
	}
}

void console_print(const char *str) {
	while (*str) {
		console_putc(*str++);
	}
}

void console_println(const char *str) {
	while (*str) {
		console_putc(*str++);
	}
	console_putc('\n');
}

void console_print_dec(uint32_t i) {
	console_print(itoa(i, 10));
}

void console_print_hex(uint32_t i) {
	console_print(itoa(i, 16));
}

void console_input(char *buf, uint32_t max_len) {
	cursor_enabled = 1;
	uint32_t idx = 0;
	uint32_t last_loop_tick = 0;
	while (1) {
		uint32_t now = uptime_ticks();

		if (now != last_loop_tick) {
			last_loop_tick = now;
			console_update();

			if (kbd_has_char()) {
				char c = kbd_read_char();
				if (c == '\0') {
					continue;
				} else if  (c == '\n') {
					console_putc('\n');
					buf[idx] = '\0';
					cursor_drawn = 0;
					erase_cursor();
					return;
				} else if (c == '\b') {
					if (idx > 0) {
						idx--;
						console_putc('\b');
					}
				} else if (idx < max_len - 1) {
					console_putc(c);
					buf[idx++] = c;
				}
			}
		}
	}

	cursor_enabled = 0;
}


void console_set_color(uint32_t color) {
	text_color = color;
}

void console_set_background_color(uint32_t color) {
	background_color = color;
}

void console_set_cursor(uint32_t x, uint32_t y) {
	cursor_x = x;
	cursor_y = y;
}

void console_clear() {
	for (uint32_t y = 0; y < fb_height; ++y) {
		for (uint32_t x = 0; x < fb_width; ++x) {
			put_pixel(x, y, background_color);
		}
	}
	cursor_x = 0;
	cursor_y = 0;
}

void draw_cursor() {
	for (uint32_t y = 0; y < FONT_HEIGHT; ++y) {
		for (uint32_t x = 0; x < FONT_WIDTH; ++x) {
			put_pixel(cursor_x * FONT_WIDTH + x, cursor_y * FONT_HEIGHT + y, text_color);
		}
	}
}

void erase_cursor() {
	draw_char(cursor_x * FONT_WIDTH, cursor_y * FONT_HEIGHT, text_color, background_color, ' ');
}

void console_update() {
	if (!cursor_enabled)
		return;
	
	uint32_t now = uptime_ticks();

	if (now - last_blink_tick >= blink_interval_ticks) {
		last_blink_tick = now;

		if (cursor_drawn) {
			erase_cursor();
			cursor_drawn = 0;
		} else {
			cursor_drawn = 1;
			draw_cursor();
		}
	}
}