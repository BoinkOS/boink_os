#include "panic_console.h"
#include "shell.h"
#include "../../interrupts/idt.h"
#include "../../utils.h"

void panic_shell(registers_t* regs) {
	pshell_println("~ Interactive Panic Debug Shell (use `help` to see commands) ~");
	
	char input[128];
	while (1) {
		pshell_print("pshell> ");
		pshell_input(input, sizeof(input));
		if (strcmp(input, "help") == 0) {
			cmd_help();
		} else if (strcmp(input, "regs") == 0) {
			dump_registers(regs);
		} else if (strcmp(input, "bt") == 0) {
			cmd_backtrace(regs->ebp);
		} else if (strncmp(input, "memdump", 7) == 0) {
			uint32_t addr, len;
			if (parse_memdump_args(input, &addr, &len)) {
				cmd_memdump(addr, len);
			} else {
				pshell_println("usage: memdump <addr> [len]");
			}
		} else if (strcmp(input, "whereami") == 0) {
			cmd_whereami(regs);
		} else {
			pshell_print("unknown command: ");
			pshell_print(input);
		}
		pshell_putc('\n');
	}
}

void cmd_backtrace(uint32_t ebp) {
	pshell_println("=== Stack Backtrace ===");
	for (int i = 0; i < 10 && ebp; ++i) {
		uint32_t ret_addr = *((uint32_t*)(ebp + 4));
		pshell_print("  ["); pshell_print_dec(i); pshell_print("] ");
		pshell_print("0x"); pshell_print_hex(ret_addr); pshell_putc('\n');
		ebp = *((uint32_t*)ebp);
	}
}

void cmd_help() {
	pshell_println("Commands:");
	pshell_println("  regs        - dump CPU registers at panic time");
	pshell_println("  bt          - show call stack");
}

void cmd_memdump(uint32_t addr, uint32_t len) {
	if (addr < 0x1000 || addr > 0xC0000000) {
		pshell_println("address in invalid range.");
		return;
	}
	
	const uint8_t* mem = (const uint8_t*)addr;

	for (uint32_t i = 0; i < len; i += 16) {
		// address label
		pshell_print("0x");
		pshell_print_hex(addr + i);
		pshell_print(":  ");

		// hex values
		for (uint32_t j = 0; j < 16 && i + j < len; ++j) {
			uint8_t byte = mem[i + j];
			pshell_print_hex(byte);
			pshell_print(" ");
		}

		// spacing
		pshell_print(" ");

		// ascii representation
		for (uint32_t j = 0; j < 16 && i + j < len; ++j) {
			uint8_t byte = mem[i + j];
			if (byte >= 32 && byte <= 126)
				pshell_putc(byte);
			else
				pshell_putc('.');
		}

		pshell_putc('\n');
	}
}

void cmd_whereami(registers_t* regs) {
	pshell_print("EIP: 0x"); pshell_print_hex(regs->eip); pshell_putc('\n');
	pshell_print(" CS: 0x"); pshell_print_hex(regs->cs);  pshell_putc('\n');
}


int parse_memdump_args(const char* input, uint32_t* addr, uint32_t* len) {
	// input format: "memdump 0xADDR LEN"
	char* ptr = (char*)input;

	// skip past command name
	while (*ptr && *ptr != ' ') ptr++;
	if (!*ptr) return 0;
	ptr++;

	// parse addr
	*addr = strtol(ptr, &ptr, 0);
	if (!*ptr) {
		*len = 16; // default length
		return 1;
	}

	// parse optional len
	*len = strtol(ptr, NULL, 0);
	return 0;
}

