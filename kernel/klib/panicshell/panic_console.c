#include "panic_console.h"
#include "../../drivers/video/fb.h"
#include "../../drivers/video/drawing.h"
#include "../../drivers/video/text.h"
#include "../../utils.h"
#include "../input/keyboard/keyboard.h"

static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;

static const uint32_t text_color = 0xFFFFFF;
static const uint32_t background_color = 0xFF0000;

static const uint32_t margin_left = 200;
static uint32_t margin_top = 200;
static const uint32_t margin_right = 200;
static const uint32_t margin_bottom = 194;
static const uint32_t title_height = 20;

static const uint32_t title_text_color = 0xFFFFFF;
static const uint32_t title_background_color = 0x000000;
static const uint32_t title_border_tl_color = 0xAAAAAA;
static const uint32_t title_border_rb_color = 0x555555;

static uint32_t drawable_height = 0;
static uint32_t max_cols = 0;
static uint32_t max_rows = 0;
static uint32_t scroll_step = 0;

void pshell_init() {
	cursor_x = 0;
	cursor_y = 0;

	drawable_height = fb_height - margin_top - margin_bottom;
	max_rows = drawable_height / FONT_HEIGHT;
	scroll_step = (drawable_height % FONT_HEIGHT) + FONT_HEIGHT;
	
	max_cols = (fb_width - margin_left - margin_right) / FONT_WIDTH;

	pshell_set_title("System Panic");
	pshell_clear();
}

void pshell_set_title(const char *str) {
	uint32_t border_thickness = 2;
	uint32_t title_x = margin_left;
	uint32_t title_y = margin_top - title_height - (2 * border_thickness);
	uint32_t title_width = fb_width - margin_left - margin_right;
	uint32_t title_total_height = title_height + (2 * border_thickness);

	draw_filled_rect(title_x, title_y, title_width, title_total_height, title_background_color);

	uint32_t max_chars = title_width / FONT_WIDTH;
	uint32_t text_len = strlen_max(str, max_chars);
	uint32_t text_x = title_x + (title_width - text_len * FONT_WIDTH) / 2;
	uint32_t text_y = title_y + (title_total_height - FONT_HEIGHT) / 2;

	for (uint32_t i = 0; i < text_len; ++i) {
		draw_char(text_x + i * FONT_WIDTH, text_y, title_text_color, title_background_color, str[i]);
	}

	draw_rect(title_x, title_y, title_width, title_total_height, border_thickness, title_border_tl_color, title_border_rb_color);
}

void pshell_clear() {
	for (uint32_t y = margin_top; y < fb_height - margin_bottom; ++y) {
		for (uint32_t x = margin_left; x < fb_width - margin_right; ++x) {
			put_pixel(x, y, background_color);
		}
	}
	cursor_x = 0;
	cursor_y = 0;
}

static void pshell_scroll() {
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

void pshell_putc(char c) {
	if (c == '\n') {
		cursor_x = 0;
		cursor_y++;
		if (cursor_y >= max_rows) {
			pshell_scroll();
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
				pshell_scroll();
				cursor_y = max_rows - 1;
			}
		}
	}
}

void pshell_print(const char *str) {
	while (*str) {
		pshell_putc(*str++);
	}
}

void pshell_println(const char *str) {
	pshell_print(str);
	pshell_putc('\n');
}

void pshell_print_dec(uint32_t i) {
	pshell_print(itoa(i, 10));
}

void pshell_print_hex(uint32_t i) {
	pshell_print(itoa(i, 16));
}

void pshell_input(char *buf, uint32_t max_len) {
	uint32_t idx = 0;
	while (1) {
		if (kbd_has_char()) {
			char c = kbd_read_char();
			if (c == '\0') continue;

			if (c == '\n') {
				pshell_putc('\n');
				buf[idx] = '\0';
				return;
			} else if (c == '\b') {
				if (idx > 0) {
					idx--;
					pshell_putc('\b');
				}
			} else if (idx < max_len - 1) {
				pshell_putc(c);
				buf[idx++] = c;
			}
		}
	}
}