#include <stdint.h>
#include "../disk/disk.h"
#include "../klib/console/console.h"
#include "../klib/kscratch/kscratch.h"
#include "../utils.h"
#include "../mem/mem.h"
#include "../mem/paging.h"
#include "../mem/frame_alloc.h"
#include "glfs.h"
#include "../execs/elf/elf.h" 
#include "../cpu/user_switch.h"
#include "../execs/mess/mess.h"
#include "../execs/squint/squint.h"

int glfs_file_count = 0;

glfs_file_entry glfs_files[MAX_FILES];

KernelContext kernel_ctx;

uint8_t end_marker[] = "__END__\n";
#define GLFS_DIR_BUFFER_VADDR 0xC1000000
#define GLFS_DIR_BUFFER_PAGES 2 // enough for 1024 entries = 20k
#define GLFS_TEMP_SECTOR_BUF 0xC2000000
#define USER_BIN_BASE 0x40000000
#define USER_BIN_PAGES 4

void glfs_init_buffers() {
	for (int i = 0; i < GLFS_DIR_BUFFER_PAGES; i++) {
		uint32_t phys = alloc_frame();
		map_page(GLFS_DIR_BUFFER_VADDR + i * 0x1000, phys, PAGE_PRESENT | PAGE_RW | PAGE_KERNEL);
		flush_tlb_single(GLFS_DIR_BUFFER_VADDR + i * 0x1000);
	}
}

void glfs_map_temp_sector_buffer() {
	uint32_t phys = alloc_frame();
	map_page(GLFS_TEMP_SECTOR_BUF, phys, PAGE_PRESENT | PAGE_RW | PAGE_KERNEL);
	flush_tlb_single(GLFS_TEMP_SECTOR_BUF);
}

void* glfs_map_user_program(uint32_t size) {
	uint32_t pages = (size + 0xFFF) / 0x1000;
	uint32_t base = USER_BIN_BASE;

	for (uint32_t i = 0; i < pages; i++) {
		uint32_t phys = alloc_frame();
		map_page(base + i * 0x1000, phys, PAGE_PRESENT | PAGE_RW | PAGE_USER);
		flush_tlb_single(base + i * 0x1000);
	}

	return (void*)base;
}

int check_glfs_magic(uint8_t* buffer) {
	const char* expected = "GLFSv0\n";
	int valid = 1;

	for (int i = 0; i < 7; i++) {
		if (buffer[i] != expected[i]) {
			valid = 0;
			break;
		}
	}

	if (valid)
		return 1;
	else
		return 0;
}

void glfs_read_directory() {
	uint8_t* dir_buffer = (uint8_t*)kscratch_zero(1);
	uint8_t* sector = (uint8_t*)kscratch_zero(2);
	int sector_num = 1;
	int dir_buffer_size = 0;
	int done = 0;

	while (!done && dir_buffer_size + SECTOR_SIZE <= 0x1000) { // max 4KB
		disk_read(sector_num, sector);

		mem_cpy(dir_buffer + dir_buffer_size, sector, SECTOR_SIZE);
		dir_buffer_size += SECTOR_SIZE;

		for (int i = 0; i <= dir_buffer_size - 8; i++) {
			if (mem_cmp(&dir_buffer[i], end_marker, 8) == 0) {
				dir_buffer_size = i;
				done = 1;
				break;
			}
		}

		sector_num++;
	}

	int offset = 0;
	glfs_file_count = 0;

	while (offset + DIR_ENTRY_SIZE <= dir_buffer_size) {
		if (glfs_file_count >= MAX_FILES)
			break;

		glfs_file_entry* entry = &glfs_files[glfs_file_count];

		mem_cpy(entry->filename, &dir_buffer[offset], FILENAME_SIZE);
		entry->filename[32] = '\0';

		entry->start_sector = *((uint32_t*)(&dir_buffer[offset + 32]));
		entry->size = *((uint32_t*)(&dir_buffer[offset + 36]));

		glfs_file_count++;
		offset += DIR_ENTRY_SIZE;
	}
}

void glfs_list_files(int should_number) {
	console_println("Files on disk:");
	console_println("-------------------");

	for (int i = 0; i < glfs_file_count; i++) {
		if (should_number) {
			console_print(itoa(i+1, 10));
			console_print(". ");
		}
		console_print(glfs_files[i].filename);
		console_print(" (");
		console_print(itoa(glfs_files[i].size, 10));
		console_println(" bytes)");
	}

	console_println("-------------------");
}

void glfs_list_files_shell(int argc, char** argv) {
	glfs_list_files(1);
}

void glfs_load_file(glfs_file_entry* file, uint8_t* load_address) {
	uint32_t sector = file->start_sector;
	uint32_t remaining = file->size;

	uint8_t* buffer = (uint8_t*)GLFS_TEMP_SECTOR_BUF;
	uint8_t* dest = load_address;

	while (remaining > 0) {
		disk_read(sector, buffer);

		uint32_t bytes_to_copy = (remaining >= SECTOR_SIZE) ? SECTOR_SIZE : remaining;

		mem_cpy(dest, buffer, bytes_to_copy);

		dest += bytes_to_copy;
		sector++;
		remaining -= bytes_to_copy;
	}
}

void* glfs_map_and_load_file(const char* filename) {
	for (int i = 0; i < glfs_file_count; i++) {
		if (strcmp(glfs_files[i].filename, filename) == 0) {
			glfs_file_entry* file = &glfs_files[i];

			console_print("Mapping and loading file: ");
			console_println(file->filename);

			uint8_t* load_addr = (uint8_t*)glfs_map_user_program(file->size);
			glfs_load_file(file, load_addr);

			console_println("Program loaded into user memory.");

			return (void*)load_addr;
		}
	}

	console_println("File not found.");
	return NULL;
}

void exec_bin(const char* filename) {
	asm volatile("mov %%esp, %0" : "=r"(kernel_ctx.esp));
	glfs_init_buffers();
	glfs_map_temp_sector_buffer();
	glfs_read_directory();

	void* entry_point = glfs_map_and_load_file(filename);

	if (!entry_point) return;

	uint32_t user_stack_base = 0x500000;

	for (int i = 0; i < USER_BIN_PAGES; i++) {
		map_page(user_stack_base - i * 0x1000, alloc_frame(), PAGE_PRESENT | PAGE_RW | PAGE_USER);
		flush_tlb_single(user_stack_base - i * 0x1000);
	}

	uint32_t user_stack = user_stack_base + 0x4000 - 4;

	console_print("entry = 0x");
	console_print_hex((uint32_t)entry_point);
	console_print(", stack = 0x");
	console_print_hex(user_stack);
	console_putc('\n');
	
	uint8_t* code = (uint8_t*)entry_point;
	console_print("first 4 bytes at entry point: ");
	console_print_hex(code[0]);
	console_print(" ");
	console_print_hex(code[1]);
	console_print(" ");
	console_print_hex(code[2]);
	console_print(" ");
	console_print_hex(code[3]);
	console_print("\n");
	
	console_println("Switching to user mode...");
	console_println("---------------------------------------------");
	switch_to_user_mode((uint32_t)entry_point, user_stack);
}

void glfs_exec_bin_shell(int argc, char** argv) {
	char* fname = argv[1];
	int findex = glfs_find_file_index(fname);
	
	if (findex < 0) {
		console_println("File not found.");
		return;
	}
	
	exec_bin(fname);
}

void glfs_prompt(int argc, char** argv) {
	glfs_list_files(1);
	console_print("What file would you like to load? ");

	char input[10];
	console_set_color(0x9019ff);
	console_input(input, sizeof(input));
	console_set_color(0xffffff);

	int f = atoi(input);

	if (f <= 0 || f > glfs_file_count) {
		console_set_background_color(0xff0000);
		console_print("No file at index ");
		console_print_dec(f);
		console_print(".");
		console_set_background_color(0x000000);
		console_putc('\n');
		return;
	}

	const char* filename = glfs_files[f - 1].filename;
	console_print("File to load: ");
	console_print(filename);
	console_putc('\n');

	const char* ext = filename;
	for (int i = 0; filename[i]; i++) {
		if (filename[i] == '.') {
			ext = &filename[i + 1];
		}
	}
	
	if (!strcmp(ext, "bin")) {
		exec_bin(filename);
	} else if (!strcmp(ext, "elf")) {
		const char* args[] = { "autorun0", "hello", "world", NULL };
		exec_elf(f-1, 3, args);
	} else if (!strcmp(ext, "txt")) {
		mess(f-1, filename);
	} else if (!strcmp(ext, "bmp")) {
		squint(f-1, filename);
	} else {
		console_set_background_color(0xff0000);
		console_print("Unknown file type: .");
		console_print(ext);
		console_set_background_color(0x000000);
		console_putc('\n');
	}
}

void exec_elf(int findex, int argc, const char** argv) {
	asm volatile("mov %%esp, %0" : "=r"(kernel_ctx.esp));
	
	glfs_init_buffers();
	glfs_map_temp_sector_buffer();
	glfs_read_directory();

	glfs_file_entry* file = &glfs_files[findex];

	console_print("Loading ELF file: ");
	console_println(file->filename);

	uint8_t* elf_buf = map_temp_elf_buffer(file->size);
	glfs_load_file(file, elf_buf);

	void* entry_point = elf_load(elf_buf);

	if (!entry_point) {
		console_println("ELF load failed.");
		return;
	}
	
	uint32_t user_stack_top = 0x600000;
	size_t arg_data_size = 0;

	for (int i = 0; i < argc; i++) {
		arg_data_size += strlen(argv[i]) + 1;
	}

	size_t argv_array_size = (argc + 1) * sizeof(char*);
	size_t total_stack_needed = arg_data_size + argv_array_size + sizeof(char**) + sizeof(int);
	size_t total_mapped_stack = ((total_stack_needed + 0x10000 + 0xFFF) & ~0xFFF); // 64kb buffer
	uint32_t stack_bottom = user_stack_top - total_mapped_stack;

	// map the stack pages
	for (uint32_t addr = stack_bottom; addr < user_stack_top; addr += 0x1000) {
		map_page(addr, alloc_frame(), PAGE_PRESENT | PAGE_RW | PAGE_USER);
		flush_tlb_single(addr);
		memset((void*)addr, 0, 0x1000); 
	}

	uint32_t user_stack = user_stack_top;

	char* arg_strings[argc];

	// copy strings
	for (int i = argc - 1; i >= 0; i--) {
		size_t len = strlen(argv[i]) + 1;
		user_stack -= len;
		mem_cpy((void*)user_stack, argv[i], len);
		arg_strings[i] = (char*)user_stack;
	}

	// write argv[] array
	user_stack -= (argc + 1) * sizeof(char*);
	char* argv_array_addr = (char*)user_stack;

	for (int i = 0; i < argc; i++) {
		uint32_t ptr = (uint32_t)arg_strings[i];
		mem_cpy((void*)(user_stack + i * 4), &ptr, sizeof(uint32_t));
	}
	uint32_t null_ptr = 0;
	mem_cpy((void*)(user_stack + argc * 4), &null_ptr, sizeof(uint32_t));

	user_stack &= ~0xF;

	user_stack -= sizeof(char**);
	mem_cpy((void*)user_stack, &argv_array_addr, sizeof(char**));
	
	user_stack -= sizeof(int);
	mem_cpy((void*)user_stack, &argc, sizeof(int));
	
	// stack canary for debugging (0x5EE5B14D == "sees bird")
	uint32_t canary = 0x5EE5B14D;
	mem_cpy((void*)(user_stack - 4), &canary, 4);
	
	console_print("entry point: 0x");
	console_print_hex((uint32_t)entry_point);
	console_print("\nstack ptr: 0x");
	console_print_hex(user_stack);
	console_putc('\n');

	for (int i = 0; i < argc; i++) {
		console_print("arg ");
		console_print_dec(i);
		console_print(": ");
		console_print_hex((uint32_t)arg_strings[i]);
		console_print(" -> ");
		console_println(arg_strings[i]);
	}
	

	console_print("Canary: 0x");
	console_print_hex(*(uint32_t*)(user_stack - 4));
	console_print(" (ensure 0x5EE5B14D)");
	console_putc('\n');
	
	console_println("Switching to user mode...");
	console_println("---------------------------------------------");
	switch_to_user_mode((uint32_t)entry_point, user_stack);
}

void glfs_exec_elf_shell(int argc, char** argv) {
	char* fname = argv[1];
	int findex = glfs_find_file_index(fname);
	
	if (findex < 0) {
		console_println("File not found.");
		return;
	}
	
	const char* elf_argv[10];
	int elf_argc = argc - 2;

	for (int i = 0; i < elf_argc; i++) {
		elf_argv[i] = argv[i + 2];
	}

	exec_elf(findex, elf_argc, elf_argv);
}


void* glfs_load_file_to_address(int findex, uint32_t dest_addr) {
	glfs_init_buffers();
	glfs_map_temp_sector_buffer();
	glfs_read_directory();

	glfs_file_entry* file = &glfs_files[findex];

	console_print("Loading file: ");
	console_println(file->filename);

	uint32_t pages_needed = (file->size + 0xFFF) / 0x1000;

	disable_frame_debug();
	for (uint32_t j = 0; j < pages_needed; j++) {
		uint32_t phys = alloc_frame();
		map_page(dest_addr + j * 0x1000, phys, PAGE_PRESENT | PAGE_RW | PAGE_KERNEL);
		flush_tlb_single(dest_addr + j * 0x1000);
	}

	enable_frame_debug();

	uint8_t* buf = (uint8_t*)dest_addr;
	glfs_load_file(file, buf);

	console_println("File loaded to memory.");
	return buf;
}


int glfs_find_file_index(const char* filename) {
	for (int i = 0; i < glfs_file_count; i++) {
		if (strcmp(glfs_files[i].filename, filename) == 0) {
			return i;
		}
	}
	
	return -1;
}