#include "syscall.h"
#include "../klib/console/console.h"
#include "../cpu/user_switch.h"
#include "../utils.h"
#include "../klib/uptime/uptime.h"

__attribute__((noreturn))
void syscall_exit_trampoline(uint32_t esp, void* ret_addr) {
	console_print("restoring kernel ESP, value was 0x");
	console_print_hex(kernel_ctx.esp);
	console_print(".");
	console_putc('\n');
	
	console_print("return address is 0x");
	console_print_hex((uint32_t)kernel_ctx.ret_addr);
	console_print(".");
	console_putc('\n');
	
	__asm__ __volatile__("sti");
	console_println("interrupts have been re-enabled.\n");
	
	asm volatile (
		"mov %0, %%esp\n"
		"jmp *%1\n"
		:
		: "r"(esp), "r"(ret_addr)
		: "memory"
	);
	__builtin_unreachable();
}

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

	return handle_syscall(syscall_num, arg1, arg2, arg3, arg4, arg5);
}


int handle_syscall(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5) {
	switch (num) {
		case SYSCALL_EXIT:
			console_print("process exited with code ");
			console_print_dec(arg1);
			console_print(".");
			console_putc('\n');
			syscall_exit_trampoline(kernel_ctx.esp, kernel_ctx.ret_addr);
			__builtin_unreachable();

		case SYSCALL_PUTCHAR:
			console_putc((char)arg1);
			return 1;

		case SYSCALL_GETCHAR:
			__asm__ __volatile__("sti");
			char c = read_key();
			return (int)c;
		
		case SYSCALL_SLEEP:
			__asm__ __volatile__("sti");
			sleep((int)arg1);
			return 1;
		
		case SYSCALL_UPTIME:
			uint32_t ticks = uptime_seconds();
			return (int)ticks;
		
		case SYSCALL_INPUT:
			__asm__ __volatile__("sti");
			char *buf = (char *)arg1;
			uint32_t max_len = arg2;
			console_input(buf, max_len);
			return 1;
		
		case SYSCALL_CLEAR:
			console_clear();
			return 1;
		
		

		default:
			console_set_background_color(0xFF0000);
			console_set_color(0xFFFFFF);
			console_putc('\n');
			console_putc('\n');
			console_print("Sorry, this program was terminated by the kernel.\n    unimplemented trap (");
			console_print_dec(num);
			console_println(")");
			console_print("roses are red,\nviolets are blue,\numimplemented trap!\nguess that one's on you.");
			console_set_background_color(0x000000);
			console_putc('\n');
			console_putc('\n');

			syscall_exit_trampoline(kernel_ctx.esp, kernel_ctx.ret_addr);
			__builtin_unreachable();
	}
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