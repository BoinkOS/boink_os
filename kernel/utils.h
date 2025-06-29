#include <stdint.h>

char* itoa(uint32_t val, int base);
int strlen(const char * str);
char read_key();
void sleep(uint32_t ticks);
int strcmp(const char* s1, const char* s2);
int atoi(const char* str);
void dump_hex_range(uint32_t from, uint32_t to);