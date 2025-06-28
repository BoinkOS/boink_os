#include <stdint.h>

#define EI_NIDENT 16
#define PT_LOAD   1
#define ELF_TEMP_LOAD 0xC3000000
#define PIE_BASE_ADDR 0x400000

#define ET_EXEC 2
#define ET_DYN 3
#define EM_386 3

#define PT_LOAD 1

#define PF_X 1  // execute
#define PF_W 2  // write
#define PF_R 4  // read

typedef struct {
	uint8_t  e_ident[EI_NIDENT];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
} Elf32_Ehdr;

typedef struct {
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
} Elf32_Phdr;

void* elf_load(void* elf_data);
void* map_temp_elf_buffer(uint32_t size);