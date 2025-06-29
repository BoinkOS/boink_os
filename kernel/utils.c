// utils.c
#include "utils.h"
#include "klib/uptime/uptime.h"
#include "klib/input/keyboard/keyboard.h"
#include "klib/console/console.h"
#include <stddef.h>
#include <stdint.h>

char* itoa(uint32_t val, int base){
	static char buf[32] = {0};
	int i = 30;
	if (val == 0) {
		buf[i] = '0';
		return &buf[i];
	}
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789ABCDEF"[val % base];
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

int strcmp(const char* s1, const char* s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int atoi(const char* str) {
	int num = 0;
	int i = 0;
	bool is_negative = false;
	
	if(str[i] == '-'){
		is_negative = true;
		i++;
	}
	
	while (str[i] && (str[i] >= '0' && str[i] <= '9')){
		num = num * 10 + (str[i] - '0');
		i++;
	}
	
	if(is_negative) num = -1 * num;
	return num;
}

void dump_hex_range(uint32_t from, uint32_t to) {
	console_println("dumping user stack:");

	for (uint32_t addr = from; addr < to; addr += 16) {
		console_print_hex(addr);
		console_putc(':');
		console_putc(' ');

		for (int i = 0; i < 16; i++) {
			uint8_t byte = *((uint8_t*)addr);
			console_print_hex(byte);
			console_putc(' ');
			addr++;
		}

		console_putc('\n');
		addr -= 16;
	}
}