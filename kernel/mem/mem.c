#include <stdint.h>

void mem_cpy(char * dest, char * src, int num_of_bytes) {
	int i;
	for (i = 0; i < num_of_bytes; ++i)
		*(dest + i) = *(src + i);
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
