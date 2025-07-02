// utils.c
#include "utils.h"
#include "klib/uptime/uptime.h"
#include "klib/input/keyboard/keyboard.h"
#include "klib/console/console.h"
#include "klib/panicshell/panic_console.h"
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

int strncmp(const char *s1, const char *s2, size_t n) {
	for (size_t i = 0; i < n; i++) {
		if (s1[i] != s2[i])
			return (unsigned char)s1[i] - (unsigned char)s2[i];
		if (s1[i] == '\0')
			return 0;
	}
	return 0;
}

int atoi(const char* str) {
	int num = 0;
	int i = 0;
	bool is_negative = 0;
	
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

void dump_hex_range_to_pshell(uint32_t from, uint32_t to) {
	for (uint32_t addr = from; addr < to; addr += 16) {
		pshell_print_hex(addr);
		pshell_putc(':');
		pshell_putc(' ');

		for (int i = 0; i < 16; i++) {
			uint8_t byte = *((uint8_t*)addr);
			pshell_print_hex(byte);
			pshell_putc(' ');
			addr++;
		}

		pshell_putc('\n');
		addr -= 16;
	}
}

uint32_t strlen_max(const char *str, uint32_t max) {
	uint32_t len = 0;
	while (*str++ && len < max) {
		len++;
	}
	return len;
}

long strtol(const char *str, char **endptr, int base) {
	long result = 0;
	int negative = 0;

	// skip whitespace
	while (*str == ' ' || *str == '\t' || *str == '\n') str++;

	// handle sign
	if (*str == '-') {
		negative = true;
		str++;
	} else if (*str == '+') {
		str++;
	}

	// auto-detect base
	if ((base == 0 || base == 16) &&
	    str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		str += 2;
		base = 16;
	} else if (base == 0 && str[0] == '0') {
		base = 8;
	} else if (base == 0) {
		base = 10;
	}

	while (*str) {
		char c = *str;
		int digit;

		if (c >= '0' && c <= '9')
			digit = c - '0';
		else if (c >= 'a' && c <= 'f')
			digit = c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			digit = c - 'A' + 10;
		else
			break;

		if (digit >= base) break;

		result = result * base + digit;
		str++;
	}

	if (endptr)
		*endptr = (char*)str;

	return negative ? -result : result;
}

char* strtok(char* str, const char* delim) {
	static char* next = 0;

	if (str) {
		next = str;
	}

	if (!next) return 0;

	// skip any leading delimiters
	while (*next && strchr(delim, *next)) {
		next++;
	}

	if (!*next) return 0;

	char* token_start = next;

	// find end of token
	while (*next && !strchr(delim, *next)) {
		next++;
	}

	if (*next) {
		*next = '\0';
		next++; // move past null
	} else {
		next = 0; // no more tokens
	}

	return token_start;
}

char* strchr(const char* str, int c) {
	while (*str) {
		if (*str == (char)c)
			return (char*)str;
		str++;
	}
	return 0;
}

char* strcat(char* dest, const char* src) {
	char* original = dest;

	while (*dest != '\0') {
		dest++;
	}
	
	while (*src != '\0') {
		*dest = *src;
		dest++;
		src++;
	}

	*dest = '\0';
	return original;
}

uint32_t parse_hex(const char* str) {
	uint32_t result = 0;

	// skip "0x" or "0X" if present
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		str += 2;
	}

	while (*str) {
		result <<= 4;

		if (*str >= '0' && *str <= '9')
			result |= (*str - '0');
		else if (*str >= 'A' && *str <= 'F')
			result |= (*str - 'A' + 10);
		else if (*str >= 'a' && *str <= 'f')
			result |= (*str - 'a' + 10);
		else
			break; // invalid char

		str++;
	}

	return result;
}