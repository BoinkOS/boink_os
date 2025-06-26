// syscall.c
#include "syscall.h"
#include "../klib/console/console.h"

int syscall_handler(uint32_t int_num, uint32_t error_code) {
	uint32_t syscall_num;
	asm volatile("mov %%eax, %0" : "=r"(syscall_num));

	if (syscall_num == SYSCALL_PRINT) {
		console_print("syscall print called!\n");
		return 1; // handled
	}

	console_print("unknown syscall\n");
	return 1;
}
