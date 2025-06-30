#include "panic_console.h"
#include "shell.h"
#include "../../interrupts/idt.h"
#include "../../utils.h"

void cmd_help();
void cmd_backtrace(uint32_t ebp);
void cmd_memdump(const char* arg1, const char* arg2);
void cmd_whereami(registers_t* regs);

void panic_shell(registers_t* regs) {
	pshell_println("~ Interactive Panic Debug Shell (use `help` to see commands) ~");
	
	char input[128];
	while (1) {
		pshell_print("pshell> ");
		pshell_input(input, sizeof(input));
		
		char* cmd = strtok(input, " ");
		char* arg1 = strtok(NULL, " ");
		char* arg2 = strtok(NULL, " ");
		
		if (!cmd) continue;
		
		pshell_set_title(input);
		
		if (strcmp(cmd, "help") == 0) {
			cmd_help();
		} else if (strcmp(cmd, "regs") == 0) {
			dump_registers(regs);
		} else if (strcmp(cmd, "bt") == 0) {
			cmd_backtrace(regs->ebp);
		} else if (strcmp(cmd, "memdump") == 0) {
			if (!arg1 || !arg2) pshell_println("usage: memdump <start> <end>");
			else cmd_memdump(arg1, arg2);
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
	pshell_println("  regs                   - dump CPU registers at panic time");
	pshell_println("  bt                     - show call stack");
	pshell_println("  memdump <s> <e>        - dump memory from 0x<s> to 0x<e>");
	pshell_println("  whereami               - dump eip and cs");
}

static uint32_t parse_hex(const char* str) {
	uint32_t result = 0;

	// skip "0x" or "0X" if present
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		str += 2;
	}

	while (*str) {
		result <<= 4;

		if (*str >= '0' && *str <= '9')
			result |= (*str - '0');
		else if (*str >= 'A' && *str <= 'F')
			result |= (*str - 'A' + 10);
		else if (*str >= 'a' && *str <= 'f')
			result |= (*str - 'a' + 10);
		else
			break; // invalid char

		str++;
	}

	return result;
}

void cmd_memdump(const char* arg1, const char* arg2) {
	uint32_t start = parse_hex(arg1);
	uint32_t end = parse_hex(arg2);

	if (start >= end || end - start > 0x1000) {
		pshell_println("invalid range or too large (max 0x1000)");
		return;
	}

	dump_hex_range_to_pshell(start, end);
}

void cmd_whereami(registers_t* regs) {
	pshell_print("EIP: 0x"); pshell_print_hex(regs->eip); pshell_putc('\n');
	pshell_print(" CS: 0x"); pshell_print_hex(regs->cs);  pshell_putc('\n');
}