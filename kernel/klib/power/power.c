#include "../../drivers/video/fb.h"
#include "../../drivers/video/drawing.h"
#include "../../drivers/video/text.h"
#include "../console/console.h"
#include "../../llio.h"
#include "../../utils.h"
#include <stdint.h>

static uint32_t text_color = 0xFFFFFF;
static uint32_t background_color = 0x000000;

static uint32_t margin_left = 0;
static uint32_t margin_top = 0;
static uint32_t w = 360;
static uint32_t h = 120;

static uint32_t screen_background_color = 0x9185BC;
static uint32_t title_background_color = 0x000000;
static uint32_t title_border_tl_color = 0xAAAAAA;
static uint32_t title_border_rb_color = 0x555555;

int is_shutting_down = 0;

void draw_message() {
	draw_filled_rect(0, 0, fb_width, fb_height, screen_background_color);
	draw_filled_rect(margin_left, margin_top, w, h, 0x000000);
	draw_rect(margin_left, margin_top, w, h, 2, title_border_tl_color, title_border_rb_color);
	
	const char *messageL1 = "It is now safe to power off your system.";
	const char *messageL2 = "Boink Operating System";
	
	uint32_t text_len = strlen(messageL1); 
	
	uint32_t text_x = (fb_width - text_len * FONT_WIDTH) / 2;
	uint32_t text_y = (fb_height - FONT_HEIGHT) / 2;

	for (uint32_t i = 0; i < text_len; ++i) {
		draw_char(text_x + i * FONT_WIDTH, text_y, 0xffffff, 0x000000, messageL1[i]);
	}
	
	text_len = strlen(messageL2); 
	
	text_x = (fb_width - text_len * FONT_WIDTH) / 2;
	text_y = fb_height - FONT_HEIGHT - 50;

	for (uint32_t i = 0; i < text_len; ++i) {
		draw_char(text_x + i * FONT_WIDTH, text_y, 0x5c5478, screen_background_color, messageL2[i]);
	}
	
}

void shutdown_cmd(int argc, char** argv) {
	is_shutting_down = 1;
	margin_left = (fb_width - w)/2;
	margin_top = (fb_height - h)/2;
	
	__asm__ volatile ("cli");

	struct {
		uint16_t limit;
		uint32_t base;
	} __attribute__((packed)) null_idt = { 0, 0 };

	__asm__ volatile ("lidt %0" : : "m"(null_idt));

	pbout(0x21, 0xFF);
	pbout(0xA1, 0xFF);	
	
	draw_message();
}