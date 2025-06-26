// syscall.c
#include "syscall.h"
#include "../klib/console/console.h"

int syscall_handler(uint32_t int_num, uint32_t error_code) {
	uint32_t num, a, b, c;
	asm volatile(
		"mov %%eax, %0\n"
		"mov %%ebx, %1\n"
		"mov %%ecx, %2\n"
		"mov %%edx, %3\n"
		: "=r"(num), "=r"(a), "=r"(b), "=r"(c)
	);

	switch (num) {
		case SYSCALL_PRINT:
			console_print("syscall print called!\n");
			return 0;

		case SYSCALL_EXIT:
			console_print("process exited\n");
			for (;;);
			break;

		// sample syscall with args
		case 3: // e.g. sum
			return a + b + c;
	}

	console_print("unknown syscall\n");
	return -1;
}

int syscall(int num, int arg1, int arg2, int arg3) {
	int ret;
	asm volatile(
		"int $0x80"
		: "=a"(ret)
		: "a"(num), "b"(arg1), "c"(arg2), "d"(arg3)
	);
	return ret;
}