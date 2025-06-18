#include <stdint.h>
#include "../disk/disk.h"
#include "../drivers/video/vga_text.h"
#include "../utils.h"
#include "../mem/mem.h"
#include "glfs.h"

glfs_file_entry glfs_files[MAX_FILES];
int glfs_file_count = 0;

uint8_t dir_buffer[SECTOR_SIZE * 256]; 
int dir_buffer_size = 0;

uint8_t end_marker[] = "__END__\n";

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
	uint8_t sector[SECTOR_SIZE];
	int sector_num = 1;
	
	int done = 0;

	dir_buffer_size = 0;

	while (!done) {
		disk_read(sector_num, sector);

		for (int i = 0; i < SECTOR_SIZE; i++) {
			dir_buffer[dir_buffer_size++] = sector[i];
		}

		if (dir_buffer_size >= 8) { 
			for (int i = 0; i <= dir_buffer_size - 8; i++) {
				if (mem_cmp(&dir_buffer[i], end_marker, 8) == 0) {
					dir_buffer_size = i;
					done = 1;
				}
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
		entry->filename[32] = '\0'; // null-terminate

		entry->start_sector = *((uint32_t*)(&dir_buffer[offset + 32]));
		entry->size = *((uint32_t*)(&dir_buffer[offset + 36]));

		glfs_file_count++;
		offset += DIR_ENTRY_SIZE;
	}
}

void glfs_list_files() {
	vga_println("GLFS File Listing:");
	vga_println("-------------------");

	for (int i = 0; i < glfs_file_count; i++) {
		vga_print(itoa(i + 1, 10));
		vga_print(": ");
		vga_print(glfs_files[i].filename);
		vga_print(" | Start Sector: ");
		vga_print(itoa(glfs_files[i].start_sector, 10));

		vga_print(" | Size: ");
		vga_print(itoa(glfs_files[i].size, 10));

		vga_println(" bytes");
	}

	vga_println("-------------------");

	 vga_println("Select file to run (1 - N): ");
}

void glfs_load_file(glfs_file_entry* file, uint8_t* load_address) {
	uint32_t sector = file->start_sector;
	uint32_t remaining = file->size;

	uint8_t buffer[SECTOR_SIZE];
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
	vga_print("You pressed: ");
	vga_printchar(key);
	vga_println("");

	int selection = key - '1';

	if (selection < 0 || selection >= glfs_file_count) {
		vga_println("Invalid selection.");
		return;
	}

	glfs_file_entry* file = &glfs_files[selection];
	vga_print("Loading file: ");
	vga_println(file->filename);

	uint8_t* load_address = (uint8_t*)0x100000;
	glfs_load_file(file, load_address);

	vga_println("File loaded. Attempting to jump...");
	
	// UPER unsafe: cast to function pointer and jump
	void (*entry_point)() = (void (*)())load_address;
	entry_point();
}