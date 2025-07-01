#include "console.h"
#include "../../drivers/video/fb.h"
#include "../../drivers/video/drawing.h"
#include "../../drivers/video/text.h"
#include "../input/keyboard/keyboard.h"
#include "../uptime/uptime.h"
#include "../../utils.h"

static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;
static uint32_t text_color = 0xFFFFFF;
static uint32_t background_color = 0x000000;

static uint32_t margin_left = 24;
static uint32_t margin_top = 40;
static uint32_t margin_right = 24;
static uint32_t margin_bottom = 20;
static uint32_t title_height = 20;

static uint32_t screen_background_color = 0x9185BC;
static uint32_t title_text_color = 0xFFFFFF;
static uint32_t title_background_color = 0x000000;
static uint32_t title_border_tl_color = 0xAAAAAA;
static uint32_t title_border_rb_color = 0x555555;

uint32_t drawable_height = 0;

static uint32_t max_cols = 0;
static uint32_t max_rows = 0;

static uint8_t cursor_enabled = 1;
static uint8_t cursor_drawn = 1;
static uint32_t last_blink_tick = 0;
static uint32_t blink_interval_ticks = 50;
static uint32_t scroll_step = 0;

void console_init() {
	cursor_x = 0;
	cursor_y = 0;
	
	drawable_height = fb_height - margin_top - margin_bottom;
	max_rows = drawable_height / FONT_HEIGHT;

	scroll_step = (drawable_height % FONT_HEIGHT) + FONT_HEIGHT;
	max_cols = (fb_width - margin_left - margin_right) / FONT_WIDTH;

	draw_console_background_and_frame();
	console_clear();
}

void draw_console_background_and_frame() {
	draw_filled_rect(0, 0, fb_width, fb_height, screen_background_color);
	console_set_title("Boink Operating System");
}

void console_set_title(const char *str) {
	uint32_t border_thickness = 2;

	uint32_t title_x = margin_left;
	uint32_t title_y = margin_top - title_height - (2 * border_thickness);
	uint32_t title_width = fb_width - margin_left - margin_right;
	uint32_t title_total_height = title_height + (2 * border_thickness);
	draw_filled_rect(title_x, title_y, title_width, title_total_height, title_background_color);

	uint32_t max_chars = title_width / FONT_WIDTH;
	uint32_t text_len = strlen_max(str, max_chars);

	// center horizontally and vertically
	uint32_t text_x = title_x + (title_width - text_len * FONT_WIDTH) / 2;
	uint32_t text_y = title_y + (title_total_height - FONT_HEIGHT) / 2;

	for (uint32_t i = 0; i < text_len; ++i) {
		draw_char(text_x + i * FONT_WIDTH, text_y, title_text_color, title_background_color, str[i]);
	}

	draw_rect(title_x, title_y, title_width, title_total_height, border_thickness, title_border_tl_color, title_border_rb_color);
}



static void console_scroll() {
	uint8_t *fb = (uint8_t *)framebuffer;

	uint32_t bytes_per_pixel = 4;
	uint32_t drawable_width = fb_width - margin_left - margin_right;
	uint32_t drawable_height_pixels = drawable_height;

	for (uint32_t y = 0; y < drawable_height_pixels - scroll_step; ++y) {
		for (uint32_t x = 0; x < drawable_width; ++x) {
			uint32_t src_x = margin_left + x;
			uint32_t dst_x = margin_left + x;

			uint32_t src_y = margin_top + y + scroll_step;
			uint32_t dst_y = margin_top + y;

			copy_pixel(src_x, src_y, dst_x, dst_y);
		}
	}

	// clear the last text row
	uint32_t clear_start = margin_top + drawable_height_pixels - scroll_step;
	uint32_t clear_end = clear_start + scroll_step;

	for (uint32_t y = clear_start; y < clear_end; ++y) {
		for (uint32_t x = margin_left; x < fb_width - margin_right; ++x) {
			put_pixel(x, y, background_color);
		}
	}

	if (cursor_y > 0) {
		cursor_y--;
	}
	
	if (scroll_step != FONT_HEIGHT) {
		scroll_step = FONT_HEIGHT;
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
		draw_char(margin_left + cursor_x * FONT_WIDTH,
		          margin_top + cursor_y * FONT_HEIGHT,
		          text_color, background_color, ' ');
	} else {
		draw_char(margin_left + cursor_x * FONT_WIDTH,
		          margin_top + cursor_y * FONT_HEIGHT,
		          text_color, background_color, c);
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

void console_input_nonl(char *buf, uint32_t max_len) {
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

void console_set_frame_colors(uint32_t new_screen_bg, uint32_t new_title_text, uint32_t new_title_bg, uint32_t new_title_border_tl, uint32_t new_title_border_rb) {
	screen_background_color   = new_screen_bg;
	title_text_color          = new_title_text;
	title_background_color    = new_title_bg;
	title_border_tl_color     = new_title_border_tl;
	title_border_rb_color     = new_title_border_rb;
}

void console_set_cursor(uint32_t x, uint32_t y) {
	cursor_x = x;
	cursor_y = y;
}

void console_set_cursor_to_end() {
	cursor_x = max_cols-1;
	cursor_y = max_rows-1;
	console_putc('\n');
}

void console_clear() {
	for (uint32_t y = margin_top; y < fb_height - margin_bottom; ++y) {
		for (uint32_t x = margin_left; x < fb_width - margin_right; ++x) {
			put_pixel(x, y, background_color);
		}
	}
	cursor_x = 0;
	cursor_y = 0;
}

void draw_cursor() {
	for (uint32_t y = 0; y < FONT_HEIGHT; ++y) {
		for (uint32_t x = 0; x < FONT_WIDTH; ++x) {
			put_pixel(margin_left + cursor_x * FONT_WIDTH + x,
			          margin_top + cursor_y * FONT_HEIGHT + y,
			          text_color);
		}
	}
}

void erase_cursor() {
	draw_char(margin_left + cursor_x * FONT_WIDTH,
	          margin_top + cursor_y * FONT_HEIGHT,
	          text_color, background_color, ' ');
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

uint32_t console_get_max_cols() {
	return max_cols;
}

uint32_t console_get_max_rows() {
	return max_rows;
}