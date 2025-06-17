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
void glfs_list_files();