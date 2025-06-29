#include <stdint.h>


#define FONT_WIDTH 8
#define FONT_HEIGHT 16

void pshell_init();
void pshell_set_title(const char *str);
void pshell_clear();
void pshell_putc(char c);
void pshell_print(const char *str);
void pshell_println(const char *str);
void pshell_print_dec(uint32_t i);
void pshell_print_hex(uint32_t i);
void pshell_input(char *buf, uint32_t max_len);