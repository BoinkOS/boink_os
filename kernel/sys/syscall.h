#include <stdint.h>

// syscall numbers
#define SYSCALL_PRINT 1
#define SYSCALL_EXIT  2
// add more as needed

int syscall_handler(uint32_t int_num, uint32_t error_code);