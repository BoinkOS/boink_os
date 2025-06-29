#include <stdint.h>

// syscall numbers
#define SYSCALL_EXIT 0
#define SYSCALL_PUTCHAR 1
// add more as needed

int syscall_handler(uint32_t int_num, uint32_t error_code);
int syscall(int num, int a, int b, int c, int d, int e);