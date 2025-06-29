#include <stdint.h>
#include <stddef.h>

char* itoa(uint32_t val, int base);
int strlen(const char * str);
char read_key();
void sleep(uint32_t ticks);
int strcmp(const char* s1, const char* s2);
int atoi(const char* str);
void dump_hex_range(uint32_t from, uint32_t to);
uint32_t strlen_max(const char *str, uint32_t max);
long strtol(const char *str, char **endptr, int base);
int strncmp(const char *s1, const char *s2, size_t n);