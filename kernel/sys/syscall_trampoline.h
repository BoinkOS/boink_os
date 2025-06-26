#include <stdint.h>

void* make_syscall_trampoline(uint32_t syscall_num);
void syscall_trampoline_init();
void test_syscall_tramp();