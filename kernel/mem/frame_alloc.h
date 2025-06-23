#include <stdint.h>
#include <stddef.h>

#define FRAME_SIZE 4096
#define TOTAL_FRAMES (1024 * 1024) // 4GB worth of frames
#define BITMAP_SIZE (TOTAL_FRAMES / 8) // 128 KB (4GB / 4KB = 1,048,576)

void disable_frame_debug();
void enable_frame_debug();
void frame_allocator_init();

uint32_t alloc_frame();
void free_frame(uint32_t addr);

void reserve_region(uint32_t addr, uint32_t length);
uint32_t get_free_memory();