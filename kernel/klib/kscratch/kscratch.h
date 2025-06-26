#include <stdint.h>

#define KSCRATCH_PAGE_COUNT 4
#define KSCRATCH_BASE_VADDR 0xC4000000 

void kscratch_init();

void* kscratch_get(uint32_t page_index); // get a pointer to scratch page i
void* kscratch_zero(uint32_t page_index); // same, but zero'd

void* kscratch_alloc(); // allocates next available page
void kscratch_free_all(); // clears state