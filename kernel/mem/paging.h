#include <stdint.h>

#define PAGE_SIZE 4096

#define PAGE_TABLES(x) (x * 1024 * PAGE_SIZE) // bytes covered

// page flags
#define PAGE_PRESENT 0x1
#define PAGE_RW      0x2
#define PAGE_USER    0x4

void paging_init();
void load_page_directory(uint32_t* page_directory);
void enable_paging();
void page_fault_handler();
void test_page_fault_panic();