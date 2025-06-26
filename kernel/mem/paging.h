#include <stdint.h>

#define PAGE_SIZE 4096

#define PAGE_TABLES(x) (x * 1024 * PAGE_SIZE) // bytes covered

// page flags
#define PAGE_KERNEL  0x0
#define PAGE_PRESENT 0x1
#define PAGE_RW      0x2
#define PAGE_USER    0x4

void paging_init();
void load_page_directory(uint32_t* page_directory);
void enable_paging();
int page_fault_handler(uint32_t int_num, uint32_t error_code);
void test_page_fault_panic();

uint32_t* get_page(uint32_t virtual_addr, int create);
void map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);
void unmap_page(uint32_t virtual_addr);
void test_paging(int should_test_page_fault);
void flush_tlb_single(uint32_t addr);