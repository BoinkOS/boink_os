#include <stdint.h>
#include <stddef.h>

void mem_cpy(void * dest, void * src, int num_of_bytes);
int mem_cmp(const void* ptr1, const void* ptr2, uint32_t count);
void* memset(void* dest, int val, size_t len);