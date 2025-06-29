#include <stdint.h>

#define FRAMEBUFFER_ADDRESS_PTR ((uint32_t*)0x7E00)
#define FRAMEBUFFER_PITCH_PTR ((uint16_t*)0x7E04)
#define FRAMEBUFFER_WIDTH_PTR ((uint16_t*)0x7E06)
#define FRAMEBUFFER_HEIGHT_PTR ((uint16_t*)0x7E08)
#define FRAMEBUFFER_BPP_PTR ((uint8_t*)0x7E0A)

extern uint32_t fb_addr;
extern uint32_t *framebuffer;
extern uint16_t fb_pitch;
extern uint16_t fb_width;
extern uint16_t fb_height;

void init_framebuffer();
uint32_t encode_color(uint8_t r, uint8_t g, uint8_t b);
void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_test_pattern();
void copy_pixel(uint32_t sx, uint32_t sy, uint32_t dx, uint32_t dy);