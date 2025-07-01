#include <stdint.h>

#define TXT_BASE 0x60000000
#define LINE_NUM_WIDTH 6
#define TAB_WIDTH 4

#define MAX_MATCHES 128

void mess(int findex, const char *fname);
void print_status_line(const char *str);
void print_file_contents_at_offset();
void scroll_down();
void scroll_up();
void search();
uint32_t count_visual_lines();
void clear_status_line();