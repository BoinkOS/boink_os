// utils.c
#include "utils.h"

#define VIDEO_MEMORY (char*)0xB8000
static int cursor = 0;
static unsigned char attr = 0x2F;

void putchar(char c) {
    char* video = VIDEO_MEMORY;

    if (c == '\n') {
        cursor += 160 - (cursor % 160);
    } else {
        video[cursor++] = c;
        video[cursor++] = attr;
    }
}

void print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }
}

void println(const char* str) {
    print(str);
    putchar('\n');
}

void set_text_color(unsigned char new_attr) {
    attr = new_attr;
}
