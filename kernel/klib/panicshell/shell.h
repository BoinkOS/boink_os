#include "../../interrupts/idt.h"

void panic_shell(registers_t* regs);


void cmd_help();
void cmd_backtrace(uint32_t ebp);
void cmd_memdump(uint32_t addr, uint32_t len);
void cmd_whereami(registers_t* regs);


int parse_memdump_args(const char* input, uint32_t* addr, uint32_t* len);