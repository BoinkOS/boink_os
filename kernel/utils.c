// utils.c
#include "utils.h"

void mem_cpy(char * src, char * dest, int num_of_bytes) {
	int i;
	
	for (i = 0; i < num_of_bytes; ++i)
		*(dest + i) = *(src + i);
}

char* itoa(int val, int base){
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i+1];
}

int strlen(const char * str) {
	int i = 0;
	while (str[i] != '\0')
		++i;	
	return i;
}