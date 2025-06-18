// utils.c
#include "utils.h"
#include "klib/input/keyboard/keyboard.h"
#include <stddef.h>

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

char read_key() {
	char c = 0;
	while (1) {
		if (kbd_has_char()) {
			c = kbd_read_char();
			if (c != NULL) {
				break;
			
		}
	}
	return c;
}