#include <stdint.h>

extern uint64_t gdt[];
extern uint16_t gdt_size;
extern void load_gdt();

void write_tss_gdt_entry(int index, uint32_t base, uint32_t limit);