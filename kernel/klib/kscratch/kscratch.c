#include "kscratch.h"
#include "../../mem/frame_alloc.h"
#include "../../mem/paging.h"
#include "../../mem/mem.h"
#include "../../utils.h" 
#include "../console/console.h"

static uint32_t kscratch_phys_addrs[KSCRATCH_PAGE_COUNT] = { 0 };
static uint8_t kscratch_in_use[KSCRATCH_PAGE_COUNT] = { 0 };

void kscratch_init() {
	for (uint32_t i = 0; i < KSCRATCH_PAGE_COUNT; i++) {
		uint32_t phys = alloc_frame();
		uint32_t vaddr = KSCRATCH_BASE_VADDR + (i * 0x1000);
		map_page(vaddr, phys, PAGE_PRESENT | PAGE_RW | PAGE_KERNEL);
		flush_tlb_single(vaddr);
		kscratch_phys_addrs[i] = phys;
		kscratch_in_use[i] = 0;
	}
}

void* kscratch_get(uint32_t page_index) {
	if (page_index >= KSCRATCH_PAGE_COUNT) return 0;
	kscratch_in_use[page_index] = 1;
	return (void*)(KSCRATCH_BASE_VADDR + (page_index * 0x1000));
}

void* kscratch_zero(uint32_t page_index) {
	void* ptr = kscratch_get(page_index);
	if (!ptr) return 0;
	console_print("zeroing buf at vaddr: ");
	console_print_hex((uint32_t)ptr);
	console_print(" -> phys: ");
	console_print_hex(kscratch_phys_addrs[page_index]);
	console_println("");
	memset(ptr, 0, 0x1000);
	
	return ptr;
}

void* kscratch_alloc() {
	for (uint32_t i = 0; i < KSCRATCH_PAGE_COUNT; i++) {
		if (!kscratch_in_use[i]) return kscratch_get(i);
	}
	return 0; // out of scratch pages
}

void kscratch_free_all() {
	for (uint32_t i = 0; i < KSCRATCH_PAGE_COUNT; i++) {
		kscratch_in_use[i] = 0;
	}
}