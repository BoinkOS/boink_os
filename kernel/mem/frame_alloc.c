#include "frame_alloc.h"
#include "../klib/console/console.h"
#include "../drivers/video/fb.h"

static uint8_t frame_bitmap[BITMAP_SIZE];
int is_frame_debug_enabled = 1;

#define BITMAP_INDEX(addr) ((addr) / FRAME_SIZE)
#define BITMAP_BYTE(idx) (idx / 8)
#define BITMAP_BIT(idx) (idx % 8)

void disable_frame_debug() {
	console_println("frame logging has been disabled.");
	is_frame_debug_enabled = 0;
}

void enable_frame_debug() {
	console_println("frame logging has been enabled.");
	is_frame_debug_enabled = 1;
}

static inline void set_frame(uint32_t addr) {
	uint32_t idx = BITMAP_INDEX(addr);
	frame_bitmap[BITMAP_BYTE(idx)] |= (1 << BITMAP_BIT(idx));
}

static inline void clear_frame(uint32_t addr) {
	uint32_t idx = BITMAP_INDEX(addr);
	frame_bitmap[BITMAP_BYTE(idx)] &= ~(1 << BITMAP_BIT(idx));
}

static inline uint8_t test_frame(uint32_t addr) {
	uint32_t idx = BITMAP_INDEX(addr);
	return frame_bitmap[BITMAP_BYTE(idx)] & (1 << BITMAP_BIT(idx));
}

void frame_allocator_init() {
	console_println("initializing frame allocator...");

	// zero out bitmap
	for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
		frame_bitmap[i] = 0;
	}

	// reserve first megabyte (bootloader, kernel, paging, misc)
	reserve_region(0x00000000, 0x00100000);

	// reserve paging structures (1MB + 64KB
	reserve_region(0x00100000, 0x00010000);

	// reserve framebuffer
	uint32_t fb_addr = *FRAMEBUFFER_ADDRESS_PTR;
	uint16_t fb_width = *FRAMEBUFFER_WIDTH_PTR;
	uint16_t fb_height = *FRAMEBUFFER_HEIGHT_PTR;
	uint8_t fb_bpp = *FRAMEBUFFER_BPP_PTR;

	uint32_t fb_size = fb_width * fb_height * (fb_bpp / 8);
	reserve_region(fb_addr, fb_size);

	console_print("frame allocator ready. total available (free) memory: ");
	uint32_t free_mem = get_free_memory();
	console_print_hex(free_mem);
	console_println(" bytes");
}

void reserve_region(uint32_t addr, uint32_t length) {
	if (is_frame_debug_enabled) {
		console_print("[ reserving memory ] ");
		console_print_hex(addr);
		console_print(" - ");
		console_print_hex(addr + length);
		console_println("");
	}
	uint32_t start = addr & ~(FRAME_SIZE - 1); // align down
	uint32_t end = (addr + length + FRAME_SIZE - 1) & ~(FRAME_SIZE - 1); // align up

	for (uint32_t a = start; a < end; a += FRAME_SIZE) {
		set_frame(a);
	}
}

uint32_t alloc_frame() {
	for (uint32_t i = 0; i < TOTAL_FRAMES / 8; i++) {
		if (frame_bitmap[i] != 0xFF) { // there is a free bit here
			for (uint8_t bit = 0; bit < 8; bit++) {
				if (!(frame_bitmap[i] & (1 << bit))) {
					// found free frame
					frame_bitmap[i] |= (1 << bit);
					uint32_t frame_addr = ((i * 8) + bit) * FRAME_SIZE;
					if (is_frame_debug_enabled) {
						console_print("allocated frame: ");
						console_print_hex(frame_addr);
						console_println("");
					}
					return frame_addr;
				}
			}
		}
	}

	console_set_background_color(0xFF0000);
	console_set_background_color(0xFFFFFF);
	console_println("!!! NON-FATAL PANIC !!!");
	console_println("System is out of memory.");
	while (1) asm volatile ("hlt"); // crash safely
}

void free_frame(uint32_t addr) {
	clear_frame(addr);

	console_print("freed frame: ");
	console_print_hex(addr);
	console_println("");
}

uint32_t get_free_memory() {
    uint32_t free_frames = 0;

	for (uint32_t i = 0; i < TOTAL_FRAMES; i++) {
		uint32_t byte = frame_bitmap[BITMAP_BYTE(i)];
		if (!(byte & (1 << BITMAP_BIT(i)))) {
			free_frames++;
		}
	}

	return free_frames * FRAME_SIZE; // return bytes
}