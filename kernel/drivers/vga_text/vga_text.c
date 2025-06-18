/*
 * --- BoinkOS VGA Text Mode Driver --- 
 *
 *
 */

#include "vga_text.h"
#include "../../llio.h"
#include "../../utils.h"
#include "../../mem/mem.h"
#include <stddef.h>

char attribute_byte = GRAY_ON_BLACK;

void print_char(char character, int col, int row) {
	unsigned char *vid_mem = (unsigned char *)VGA_ADDRESS;
	int offset;

	if (col >= 0 && row >= 0)
		offset = get_scr_offset(col, row);
	else
		offset = get_cursor();

	if (character == '\n') {
		int rows = offset / (2 * MAX_COLS);
		offset = get_scr_offset(0, rows + 1);
	} else {
		vid_mem[offset] = character;
		vid_mem[offset + 1] = attribute_byte;
		offset += 2;
	}

	offset = handle_scrolling(offset);
	set_cursor(offset);
}

int get_scr_offset(int col, int row) {
	return (row * MAX_COLS + col) * 2;
}

int get_cursor(void) {
	int offset;

	pbout(REG_SCR_CTRL, 0x0E);
	offset = pbin(REG_SCR_DATA) << 8;
	pbout(REG_SCR_CTRL, 0x0F);
	offset |= pbin(REG_SCR_DATA);

	return offset * 2;
}

void set_cursor(int offset) {
	offset /= 2;

	pbout(REG_SCR_CTRL, 0x0E);
	pbout(REG_SCR_DATA, (unsigned char)(offset >> 8));
	pbout(REG_SCR_CTRL, 0x0F);
	pbout(REG_SCR_DATA, (unsigned char)offset);
}

void print_at(char *str, int col, int row) {
	if (col >= 0 && row >= 0)
		set_cursor(get_scr_offset(col, row));

	while (*str)
		print_char(*str++, -1, -1);
}

void clear_screen(void) {
	unsigned char *vid_mem = (unsigned char *)VGA_ADDRESS;

	for (int i = 0; i < MAX_ROWS * MAX_COLS * 2; i += 2) {
		vid_mem[i] = ' ';
		vid_mem[i + 1] = attribute_byte;
	}

	set_cursor_to_origin();
}

void set_cursor_to_origin(void) {
	set_cursor(0);
}

int handle_scrolling(int cursor_offset) {
	if (cursor_offset < MAX_ROWS * MAX_COLS * 2)
		return cursor_offset;

	for (int row = 1; row < MAX_ROWS; ++row) {
		mem_cpy((char *)(get_scr_offset(0, row - 1) + VGA_ADDRESS),
				(char *)(get_scr_offset(0, row) + VGA_ADDRESS),
				MAX_COLS * 2);
	}

	unsigned char *last_line = (unsigned char *)(get_scr_offset(0, MAX_ROWS - 1) + VGA_ADDRESS);
	for (int i = 0; i < MAX_COLS * 2; i += 2) {
		last_line[i] = ' ';
		last_line[i + 1] = attribute_byte;
	}

	return cursor_offset - (2 * MAX_COLS);
}

char get_char_at(int row, int col) {
	unsigned char *vid_mem = (unsigned char *)VGA_ADDRESS;
	int offset = get_scr_offset(col, row);
	return vid_mem[offset];
}

void set_attribute_byte(char attrib) {
	attribute_byte = attrib;
}

char get_attribute_byte(void) {
	return attribute_byte;
}

void hide_cursor(void) {
	pwout(REG_SCR_CTRL, HIDE_CURSOR);
}

void vga_print(const char *str) {
	while (*str)
		print_char(*str++, -1, -1);
}

void vga_println(const char *str) {
	vga_print(str);
	print_char('\n', -1, -1);
}

void vga_printchar(char character) {
	if (character == '\0')
		return;
	print_char(character, -1, -1);
}

void vga_printdec(int i) {
	vga_print(itoa(i, 10));
}
