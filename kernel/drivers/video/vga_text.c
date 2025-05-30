/*
 * --- BoinkOS VGA Text Mode Driver --- 
 *
 *
 */

#include "vga_text.h"
#include "../../llio.h"
#include "../../utils.h"

char attribute_byte = GRAY_ON_BLACK;

void print_char(char character, int col, int row) {
	unsigned char * vid_mem = (unsigned char * ) VGA_ADDRESS;
	int offset;

	if (col >= 0 && row >= 0)
		offset = get_scr_offset(col, row);
	else
		offset = get_cursor();
	
	if ('\n' == character) {
		offset = get_scr_offset(MAX_COLS - 1, offset / (2 * MAX_COLS));
	} else {
		vid_mem[offset] = character;
		vid_mem[offset + 1] = attribute_byte;
	}

	offset += 2;
	
	offset = handle_scrolling(offset);

	set_cursor(offset);
}

int get_scr_offset(int col, int row) {
	int offset = (row * MAX_COLS + col) * 2;

	return offset;
}

int get_cursor(void) {
	int offset = 0;

	pbout(REG_SCR_CTRL, 0x0E);
	offset = pbin(REG_SCR_DATA) << 8;
	pbout(REG_SCR_CTRL, 0x0F);
	offset |= pbin(REG_SCR_DATA);

	offset *= 2;

	return offset;
}

void set_cursor(int offset) {
	offset /= 2;

	pbout(REG_SCR_CTRL, 0x0E);
	pbout(REG_SCR_DATA, (unsigned char)(offset >> 8));
	pbout(REG_SCR_CTRL, 0x0F);
	pbout(REG_SCR_DATA, (unsigned char) offset);
}

void print_at(char * str, int col, int row) {
	int i;
	bool coordinates = false;

	if (col >= 0 && row >= 0) {
		set_cursor(get_scr_offset(col, row));
		coordinates = true;
	}

	if (!coordinates)
		for (i = 0; str[i] != '\0'; ++i)
			print_char(str[i], col, row);
	else
		for (i = 0; str[i] != '\0'; ++i, ++col)
			print_char(str[i], col, row);

}

void clear_screen(void) {
	for (int row = 0; row < MAX_ROWS; ++row) {
		for (int col = 0; col < MAX_COLS; ++col) {
			print_char(' ', col, row);
		}
	}
	set_cursor_to_origin();
}

void set_cursor_to_origin(void) {
	set_cursor(get_scr_offset(0, 0));
}

int handle_scrolling(int cursor_offset) {
	if (cursor_offset < MAX_ROWS * MAX_COLS * 2)
		return cursor_offset;

	int i;
	for (i = 1; i < MAX_ROWS; ++i) {
		mem_cpy((char * )(get_scr_offset(0, i) + VGA_ADDRESS),
			(char * )(get_scr_offset(0, i - 1) + VGA_ADDRESS),
			MAX_COLS * 2);
	}

	char * lst_ln = (char * )(get_scr_offset(0, MAX_ROWS - 1) + VGA_ADDRESS);
	for (i = 0; i < MAX_COLS; i += 2)
		lst_ln[i] = ' ';

	cursor_offset -= 2 * MAX_COLS;

	return cursor_offset;
}

char get_char_at(int row, int col) {
	unsigned char * vid_mem = (unsigned char * ) VGA_ADDRESS;
	int offset = get_scr_offset(row, col);

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

void vga_print(const char* str) {
    while (*str) {
        print_char(*str++, -1, -1);
    }
}

void vga_println(const char* str) {
    vga_print(str);
    print_char('\n', -1, -1);
}