#include <stdint.h>
#include <stddef.h>
#include "fb.h"

uint32_t fb_addr = 0;
uint32_t *framebuffer = 0;
uint16_t fb_pitch = 2560;
uint16_t fb_width = 640;
uint16_t fb_height = 480;
uint8_t fb_bpp = 32;

void init_framebuffer() {
	fb_addr = *FRAMEBUFFER_ADDRESS_PTR;
	fb_pitch = *FRAMEBUFFER_PITCH_PTR;
	fb_width = *FRAMEBUFFER_WIDTH_PTR;
	fb_height = *FRAMEBUFFER_HEIGHT_PTR;
	fb_bpp = *FRAMEBUFFER_BPP_PTR;
	framebuffer = (uint32_t*)fb_addr;
}

uint32_t encode_color(uint8_t r, uint8_t g, uint8_t b) {
	return (b) | (g << 8) | (r << 16);
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
	uint8_t *fb = (uint8_t *)framebuffer;
	uint32_t bytes_per_pixel = fb_bpp / 8;
	uint32_t offset = y * fb_pitch + x * bytes_per_pixel;
	*(uint32_t *)(fb + offset) = color;
}

void copy_pixel(uint32_t sx, uint32_t sy, uint32_t dx, uint32_t dy) {
	uint8_t *fb = (uint8_t *)framebuffer;
	uint32_t bytes_per_pixel = fb_bpp / 8;
	uint32_t s_offset = sy * fb_pitch + sx * bytes_per_pixel;
	uint32_t d_offset = dy * fb_pitch + dx * bytes_per_pixel;
	
	*(uint32_t *)(fb + d_offset) = *(uint32_t *)(fb + s_offset);
}

void draw_test_pattern() {
	for (size_t y = 0; y < fb_height; ++y) {
		for (size_t x = 0; x < fb_width; ++x) {
			if (x % 50 < 25)
				put_pixel(x, y, encode_color(255, 0, 0)); // red vertical stripes
			else
				put_pixel(x, y, encode_color(0, 255, 0)); // green vertical stripes
		}
	}

	for (size_t y = 0; y < fb_height; ++y) {
		for (size_t x = 0; x < fb_width; ++x) {
		   if (y % 50 < 25)
				 put_pixel(x, y, encode_color(0, 0, 255));
		}
	}
}