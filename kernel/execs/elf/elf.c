#include "elf.h"
#include "../../klib/console/console.h"
#include "../../mem/frame_alloc.h"
#include "../../mem/paging.h"
#include "../../utils.h"
#include "../../mem/mem.h"
#include "../../fs/glfs.h"
#include "../../cpu/user_switch.h"

void* elf_load(void* elf_data) {
	Elf32_Ehdr* hdr = (Elf32_Ehdr*)elf_data;
	
	if (hdr->e_ident[0] != 0x7F || hdr->e_ident[1] != 'E' ||
		hdr->e_ident[2] != 'L' || hdr->e_ident[3] != 'F') {
		console_println("not a valid ELF file.");
		return NULL;
	}

	console_print("valid ELF file. entry point = 0x");
	console_print_hex(hdr->e_entry);
	console_putc('\n');

	Elf32_Phdr* phdrs = (Elf32_Phdr*)((uint8_t*)elf_data + hdr->e_phoff);

	for (int i = 0; i < hdr->e_phnum; i++) {
		Elf32_Phdr* ph = &phdrs[i];

		if (ph->p_type != PT_LOAD)
			continue;

		uint32_t vaddr = ph->p_vaddr;
		uint32_t memsz = ph->p_memsz;
		uint32_t filesz = ph->p_filesz;
		uint32_t offset = ph->p_offset;

		console_print("loading segment at 0x");
		console_print_hex(vaddr);
		console_print(" (");
		console_print_dec(memsz);
		console_println(" bytes)");

		uint32_t pages = (memsz + 0xFFF) / 0x1000;
		for (uint32_t p = 0; p < pages; p++) {
			uint32_t va = (vaddr & 0xFFFFF000) + p * 0x1000;
			map_page(va, alloc_frame(), PAGE_PRESENT | PAGE_RW | PAGE_USER);
			flush_tlb_single(va);
		}

		mem_cpy((void*)vaddr, (uint8_t*)elf_data + offset, filesz);
		memset((void*)(vaddr + filesz), 0, memsz - filesz);
	}

	return (void*)hdr->e_entry;
}

void* map_temp_elf_buffer(uint32_t size) {
	uint32_t base = ELF_TEMP_LOAD;
	uint32_t pages = (size + 0xFFF) / 0x1000;

	for (uint32_t i = 0; i < pages; i++) {
		uint32_t phys = alloc_frame();
		map_page(base + i * 0x1000, phys, PAGE_PRESENT | PAGE_RW | PAGE_KERNEL);
		flush_tlb_single(base + i * 0x1000);
	}

	return (void*)base;
}