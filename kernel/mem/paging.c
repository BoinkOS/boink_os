#include "paging.h"
#include "../interrupts/idt.h"
#include "../interrupts/isr.h"
#include "../klib/console/console.h"
#include "../drivers/video/fb.h"
#include "../utils.h"

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