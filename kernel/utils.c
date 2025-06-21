// utils.c
#include "utils.h"
#include "klib/uptime/uptime.h"
#include "klib/input/keyboard/keyboard.h"
#include <stddef.h>
#include <stdint.h>

char* itoa(uint32_t val, int base){
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
			if (c != '\0') {
				break;
			}
		}
	}
	return c;
}

void sleep(uint32_t ticks) {
	uint32_t now = uptime_ticks();
	uint32_t dest = now + ticks;
	
	while (dest > now) {
		now = uptime_ticks();
	} 
}