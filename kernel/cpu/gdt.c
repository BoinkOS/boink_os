#include "gdt.h"

void write_tss_gdt_entry(int index, uint32_t base, uint32_t limit) {
	uint64_t* descriptor = &gdt[index];
	limit--;

	uint64_t desc = 0;
	desc |= (limit & 0xFFFFULL);
	desc |= ((base & 0xFFFFFFULL) << 16);
	desc |= (0x89ULL << 40); // Available 32-bit TSS
	desc |= ((limit & 0xF0000ULL) << 32);
	desc |= ((base & 0xFF000000ULL) << 32);

	*descriptor = desc;
	
	load_gdt();
}