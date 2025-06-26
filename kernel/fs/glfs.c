#include <stdint.h>
#include "../disk/disk.h"
#include "../klib/console/console.h"
#include "../klib/kscratch/kscratch.h"
#include "../utils.h"
#include "../mem/mem.h"
#include "../mem/paging.h"
#include "../mem/frame_alloc.h"
#include "glfs.h"

glfs_file_entry glfs_files[MAX_FILES];
int glfs_file_count = 0;

uint8_t end_marker[] = "__END__\n";
#define GLFS_DIR_BUFFER_VADDR 0xC1000000
#define GLFS_DIR_BUFFER_PAGES 2 // enough for 1024 entries = 20k
#define GLFS_TEMP_SECTOR_BUF 0xC2000000
#define USER_BIN_BASE 0x40000000
#define USER_BIN_PAGES 16 // adjust based on file size

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

void glfs_list_files() {
	console_println("Files on disk:");
	console_println("-------------------");

	for (int i = 0; i < glfs_file_count; i++) {
		console_print(glfs_files[i].filename);
		console_print(" (");
		console_print(itoa(glfs_files[i].size, 10));
		console_println(" bytes)");
	}

	console_println("-------------------");
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

void glfs_file_loader() {
	glfs_list_files();

	char key = read_key();
	console_print("You pressed: ");
	console_putc(key);
	console_println("");

	int selection = key - '1';

	if (selection < 0 || selection >= glfs_file_count) {
		console_println("Invalid selection.");
		return;
	}

	glfs_file_entry* file = &glfs_files[selection];
	console_print("Loading file: ");
	console_println(file->filename);

	// allocate a scratch page for file loading
	uint8_t* scratch_load_addr = (uint8_t*)kscratch_zero(3);

	// only safe for files <4KB
	glfs_load_file(file, scratch_load_addr);
	console_println("File loaded.");
	console_println("Attempting to jump to entry point...");

	void (*entry_point)() = (void (*)())scratch_load_addr;
	entry_point();
}