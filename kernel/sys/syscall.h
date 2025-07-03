#include <stdint.h>

// syscall numbers
#define SYSCALL_EXIT 0
#define SYSCALL_PUTCHAR 1
#define SYSCALL_GETCHAR 2
#define SYSCALL_SLEEP 3
#define SYSCALL_UPTIME 4
#define SYSCALL_INPUT 5
#define SYSCALL_CLEAR 6


int syscall_handler(uint32_t int_num, uint32_t error_code);
int handle_syscall(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
int syscall(int num, int a, int b, int c, int d, int e);