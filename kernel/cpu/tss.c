#include "tss.h"
#include "gdt.h"
#include "../mem/mem.h"
#include "../utils.h"
#include "../klib/console/console.h"

static tss_entry_t tss;

void ltr(uint16_t selector) {
	asm volatile ("ltr %0" :: "r"(selector));
}

void tss_init(uint32_t kernel_stack) {
	memset(&tss, 0, sizeof(tss));

	tss.ss0 = 0x10; // kernel data segment selector (same as GDT entry 2)
	tss.esp0 = kernel_stack;

	// add TSS descriptor to GDT at index 5
	write_tss_gdt_entry(5, (uint32_t)&tss, sizeof(tss_entry_t));

	console_print("TSS base addr: 0x");
	console_print_hex((uint32_t)&tss);
	console_println("");

	console_print("GDT[5] raw: 0x");
	console_print_hex(((uint32_t*)&gdt[5])[1]); // high 32 bits
	console_print_hex(((uint32_t*)&gdt[5])[0]); // low 32 bits
	console_println("");

	ltr(0x28); // 5th GDT entry * 8 = 0x28
}

void set_kernel_stack(uint32_t stack) {
	tss.esp0 = stack;
}