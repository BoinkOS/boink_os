// syscall.c
#include "syscall.h"
#include "../klib/console/console.h"

int syscall_handler(uint32_t int_num, uint32_t error_code) {
	uint32_t syscall_num, arg1, arg2, arg3, arg4, arg5;
	asm volatile(
		"mov %%eax, %0\n"
		"mov %%ebx, %1\n"
		"mov %%ecx, %2\n"
		"mov %%edx, %3\n"
		"mov %%esi, %4\n"
		"mov %%edi, %5\n"
		: "=r"(syscall_num), "=r"(arg1), "=r"(arg2), "=r"(arg3),
		  "=r"(arg4), "=r"(arg5)
	);

	switch (syscall_num) {
		case SYSCALL_PUTCHAR:
			console_putc((char)arg1);
			return 1;
		case SYSCALL_EXIT:
			console_print("process exited with code ");
			console_print_dec(arg1);
			console_putc('\n');
			for (;;) asm volatile("hlt");
			break;
	}
	
	console_set_background_color(0xFF0000);
	console_set_color(0xFFFFFF);
	console_putc('\n');
	console_print("unknown syscall (");
	console_print_dec(syscall_num);
	console_print(")");
	console_set_background_color(0x000000);
	console_putc('\n');
	return -1;
}

int syscall(int num, int a, int b, int c, int d, int e) {
	int ret;
	asm volatile (
		"int $0x80"
		: "=a"(ret) // return value goes in eax
		: "a"(num), "b"(a), "c"(b), "d"(c), "S"(d), "D"(e)
		: "memory"
	);
	return ret;
}