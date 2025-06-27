#include <stdint.h>

#define SECTOR_SIZE 512
#define DIR_ENTRY_SIZE 40
#define FILENAME_SIZE 32
#define MAX_FILES 128

#define MAX_DIR_SECTORS 10

typedef struct {
	char filename[33];
	uint32_t start_sector;
	uint32_t size;
} glfs_file_entry;


int check_glfs_magic(uint8_t* buffer);
void glfs_read_directory();
void glfs_list_files(int should_number);
void glfs_load_file(glfs_file_entry* file, uint8_t* load_address);
void glfs_init_buffers();
void glfs_map_temp_sector_buffer();
void* glfs_map_user_program(uint32_t size);
void exec_bin(const char* filename);
void* glfs_map_and_load_file(const char* filename);
void glfs_prompt();