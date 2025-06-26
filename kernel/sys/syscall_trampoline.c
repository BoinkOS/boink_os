#include "../mem/paging.h"
#include "../mem/frame_alloc.h"
#include "../mem/mem.h"
#include "../klib/console/console.h"
#include "syscall.h"
#include <stdint.h>
#define MAX_SYSCALLS    128
#define TRAMPOLINE_SIZE 0x1000
#define TRAMPOLINE_BASE 0x700000
uint8_t* trampoline_base = (uint8_t*)TRAMPOLINE_BASE;

extern void switch_to_user_mode(uint32_t entry, uint32_t user_stack);

void syscall_trampoline_init() {
	uint32_t phys = alloc_frame();
	map_page((uint32_t)trampoline_base, phys, PAGE_PRESENT | PAGE_RW | PAGE_USER);
	flush_tlb_single((uint32_t)trampoline_base);
	memset(trampoline_base, 0xCC, TRAMPOLINE_SIZE);
}

void* make_syscall_trampoline(uint32_t syscall_num) {
	if (syscall_num >= MAX_SYSCALLS) return 0;

	uint32_t offset = syscall_num * 8;
	uint8_t* ptr = trampoline_base + offset;

	ptr[0] = 0xB8; // mov eax, imm32
	ptr[1] = (syscall_num & 0xFF);
	ptr[2] = (syscall_num >> 8) & 0xFF;
	ptr[3] = (syscall_num >> 16) & 0xFF;
	ptr[4] = (syscall_num >> 24) & 0xFF;
	ptr[5] = 0xCD; // int 0x80
	ptr[6] = 0x80;
	ptr[7] = 0xEB; // jmp $
	ptr[8] = 0xFE;

	return (void*)ptr;
}

void test_syscall_tramp() {
	syscall_trampoline_init();
	
	uint32_t user_stack_base = 0x500000;
	map_page(user_stack_base, alloc_frame(), PAGE_PRESENT | PAGE_RW | PAGE_USER);
	map_page(user_stack_base + 0x1000, alloc_frame(), PAGE_PRESENT | PAGE_RW | PAGE_USER);
	flush_tlb_single(user_stack_base);
	flush_tlb_single(user_stack_base + 0x1000);
	memset((void*)user_stack_base, 0, 0x2000);

	uint32_t user_stack = user_stack_base + 0x2000 - 4;
	
	void* user_func = make_syscall_trampoline(SYSCALL_PRINT);

	console_print("jumping to trampoline at 0x");
	console_print_hex((uint32_t)user_func);
	console_putc('\n');

	switch_to_user_mode((uint32_t)user_func, user_stack);
}