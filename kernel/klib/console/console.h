#include <stdint.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

void console_init();
void console_putc(char c);
void console_print(const char *str);
void console_println(const char *str);
void console_input(char *buf, uint32_t max_len);

void console_set_color(uint32_t color);
void console_set_cursor(uint32_t x, uint32_t y);
void console_set_background_color(uint32_t color);
void console_clear();