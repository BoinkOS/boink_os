#include "paging.h"
#include "frame_alloc.h"
#include "../interrupts/idt.h"
#include "../interrupts/isr.h"
#include "../klib/console/console.h"
#include "../drivers/video/fb.h"
#include "../utils.h"
#include <stddef.h>

#define PAGE_DIRECTORY_ADDR 0x00100000
#define PAGE_TABLES_ADDR	0x00101000

uint32_t* page_directory = (uint32_t*)PAGE_DIRECTORY_ADDR;

void paging_init() {
	console_println("initializing paging...");

	uint32_t framebuffer_addr = *FRAMEBUFFER_ADDRESS_PTR;
	uint16_t framebuffer_width = *FRAMEBUFFER_WIDTH_PTR;
	uint16_t framebuffer_height = *FRAMEBUFFER_HEIGHT_PTR;
	uint8_t framebuffer_bpp = *FRAMEBUFFER_BPP_PTR;

	uint32_t framebuffer_size = framebuffer_width * framebuffer_height * (framebuffer_bpp / 8);
	uint32_t framebuffer_end = framebuffer_addr + framebuffer_size;

	// map enough memory to cover framebuffer + some extra for kernel and stack
	uint32_t memory_to_map = framebuffer_end + (4 * 1024 * 1024); // +4MB buffer

	uint32_t num_tables = (memory_to_map + (PAGE_SIZE * 1024) - 1) / (PAGE_SIZE * 1024);
	uint32_t* page_tables = (uint32_t*)PAGE_TABLES_ADDR;

	console_print("mapping memory: ");
	console_print_hex(memory_to_map);
	console_println(" bytes");

	for (int i = 0; i < 1024; i++) {
		page_directory[i] = 0x00000002;
	}

	for (uint32_t table = 0; table < num_tables; table++) {
		uint32_t* page_table = page_tables + (table * 1024);

		for (uint32_t i = 0; i < 1024; i++) {
			uint32_t address = (table * 1024 + i) * PAGE_SIZE;
			page_table[i] = address | PAGE_PRESENT | PAGE_RW;
		}

		page_directory[table] = ((uint32_t)page_table) | PAGE_PRESENT | PAGE_RW;
	}

	load_page_directory(page_directory);
	enable_paging();

	console_println("paging enabled!\n");
}

void load_page_directory(uint32_t* pd) {
	asm volatile("mov %0, %%cr3" :: "r"(pd));
}

void enable_paging() {
	uint32_t cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000; // enable paging
	asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

void page_fault_handler() {
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");
	
	uint32_t faulting_address;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
	
	console_set_background_color(0xFF0000);
	console_set_color(0xFFFFFF);
	console_println("!!! PANIC !!!");
	console_print("PAGE FAULT at address: 0x");
	console_print_hex(faulting_address);
	console_println("\n\nThe kernel is now in an unrecoverable state.");
	console_println("BoinkOS needs to restart.");

	for (;;) __asm__ __volatile__("hlt");

	__asm__ __volatile__("popa; leave; iret");
}

uint32_t* get_page(uint32_t virtual_addr, int create) {
	uint32_t dir_index = (virtual_addr >> 22) & 0x3FF;
	uint32_t table_index = (virtual_addr >> 12) & 0x3FF;

	uint32_t* page_table = (uint32_t*)(page_directory[dir_index] & 0xFFFFF000);

	if (!(page_directory[dir_index] & PAGE_PRESENT)) {
		if (!create) return NULL;

		// alloc new page table
		uint32_t frame = alloc_frame();
		page_table = (uint32_t*)frame;

		// clear it
		for (int i = 0; i < 1024; i++) page_table[i] = 0;

		// set it in directory
		page_directory[dir_index] = ((uint32_t)page_table) | PAGE_PRESENT | PAGE_RW;
	}

	return &page_table[table_index];
}

void map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags) {
	uint32_t* page_entry = get_page(virtual_addr, 1);
	*page_entry = (physical_addr & 0xFFFFF000) | (flags & 0xFFF);
}

void unmap_page(uint32_t virtual_addr) {
	uint32_t* page_entry = get_page(virtual_addr, 0);
	if (page_entry) {
		*page_entry = 0;
		asm volatile("invlpg (%0)" :: "r" (virtual_addr) : "memory"); // flush TLB
	}
}

void test_paging(int should_test_page_fault) {
	console_set_color(0x9019ff);
	console_println("*-------------------------------------------------------------------*");
	console_println("* void test_paging(int should_test_page_fault);                     *");
	console_println("*                                                                   *");
	console_println("* Virtual address 0x400000 will be mapped to a page, and a test     *");
	console_println("* code will be written to it and read back.                         *");
	console_println("*                                                                   *");
	console_println("* Expect to read back 0xDEADBEEF.                                   *");
	console_println("*-------------------------------------------------------------------*");
	console_set_color(0x888888);
	
	enable_frame_debug();
	
	console_set_color(0xFFFFFF);
	console_print("free memory before: ");
	console_print_hex(get_free_memory());
	console_println(" bytes");
	
	console_set_color(0x888888);
	uint32_t phys = alloc_frame();
	map_page(0x400000, phys, PAGE_PRESENT | PAGE_RW);
	
	console_set_color(0xFFFFFF);
	console_print("mapped 0x400000 -> 0x");
	console_print_hex(phys);
	console_println("");
	
	console_print("free memory after writing: ");
	console_print_hex(get_free_memory());
	console_println(" bytes");
	
	uint32_t* test = (uint32_t*)0x400000;
	*test = 0xDEADBEEF;
	console_print("read back: 0x");
	console_print_hex(*test);
	console_println("");
	
	unmap_page(0x400000);
	console_println("page unmapped.");
	
	console_set_color(0x888888);
	free_frame(phys);
	disable_frame_debug();
	
	console_set_color(0xFFFFFF);
	console_print("free memory after freeing: ");
	console_print_hex(get_free_memory());
	console_println(" bytes");
	
	if (should_test_page_fault) {
		console_set_color(0x9019ff);
		console_println("should_test_page_fault=1");
		console_println("Testing access to unmapped memory (should panic)...");
		volatile uint32_t dead = *test; // this should trigger page fault handler
		console_print("Somehow read: ");
		console_print_hex(dead);
		console_println("");
	}
}

void test_page_fault_panic() {
	console_set_color(0x9019ff);
	console_println("*-------------------------------------------------------------------*");
	console_println("* void test_page_fault_panic();                                     *");
	console_println("*                                                                   *");
	console_println("* This will test the kernel's handling of a page fault 500 ticks    *");
	console_println("* from now by writing to 0xFFFFF000 (top of 32-bit addressing).     *");
	console_println("*                                                                   *");
	console_println("* Expect the kernel to panic and enter an unrecoverable state.      *");
	console_println("*-------------------------------------------------------------------*");
	
	sleep(500);
	
	volatile uint32_t* invalid_address = (uint32_t*)0xFFFFF000;
	uint32_t data = *invalid_address;
}