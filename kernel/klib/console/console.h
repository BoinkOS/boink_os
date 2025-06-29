#include <stdint.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

void console_init();
void draw_console_background_and_frame();
void console_set_title(const char *str);
void console_putc(char c);
void console_print(const char *str);
void console_println(const char *str);
void console_print_dec(uint32_t i);
void console_print_hex(uint32_t i);
void console_input(char *buf, uint32_t max_len);

void console_set_color(uint32_t color);
void console_set_cursor(uint32_t x, uint32_t y);
void console_set_background_color(uint32_t color);
void console_set_frame_colors(uint32_t new_screen_bg, uint32_t new_title_text, uint32_t new_title_bg, uint32_t new_title_border_tl, uint32_t new_title_border_rb);
void console_clear();

void draw_cursor();
void erase_cursor();
void console_update();
void console_set_cursor_to_end();