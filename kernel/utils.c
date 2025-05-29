// utils.c
#include "utils.h"

void mem_cpy(char * src, char * dest, int num_of_bytes) {
	int i;
	
	for (i = 0; i < num_of_bytes; ++i)
		*(dest + i) = *(src + i);
}