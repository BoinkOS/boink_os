#include <stdint.h>

void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                 uint32_t thickness,
                 uint32_t color_tl, uint32_t color_br);
void draw_filled_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);