#include <stdint.h>
#include <stddef.h>

void mem_cpy(void *dest, void *src, int num_of_bytes) {
	uint8_t *d = (uint8_t *)dest;
	uint8_t *s = (uint8_t *)src;

	for (int i = 0; i < num_of_bytes; i++) {
		d[i] = s[i];
	}
}

int mem_cmp(const void* ptr1, const void* ptr2, uint32_t count) {
	const uint8_t* p1 = (const uint8_t*)ptr1;
	const uint8_t* p2 = (const uint8_t*)ptr2;

	for (uint32_t i = 0; i < count; i++) {
		if (p1[i] != p2[i]) {
			return p1[i] - p2[i];
		}
	}

	return 0;
}

void* memset(void* dest, int val, size_t len) {
	unsigned char *ptr = (unsigned char*)dest;
	unsigned char byte = (unsigned char)val;
	for (size_t i = 0; i < len; i++) {
		ptr[i] = byte;
	}
	return dest;
}