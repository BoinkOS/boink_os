#include "../../klib/console/console.h"
#include "../../fs/glfs.h"
#include "mess.h"
#include "../../utils.h"
#include "../../mem/mem.h"

static uint32_t available_cols = 0;
static uint32_t available_rows = 0;
static char* txt = "No file loaded.";
static uint32_t scroll_offset = 0;


static uint32_t match_indices[MAX_MATCHES];
static uint32_t match_count = 0;
static uint32_t search_term_len = 0;

void mess(int findex, const char *fname) {
	available_cols = console_get_max_cols();
	available_rows = console_get_max_rows() - 1;
	scroll_offset = 0;
	
	console_println("mess has started.");
	txt = (char*)glfs_load_txt_file(findex, TXT_BASE);


	console_set_background_color(0xFFFFFF);
	console_clear();
	console_set_title("mess: a mediocre less");

	print_status_line(fname);
	print_file_contents_at_offset();
	
	char inp = ' ';
	while (inp != 'q') {
		inp = read_key();
		
		if (inp == 'w') {
			scroll_up();
		} else if (inp == 's') {
			scroll_down();
		} else if (inp == '\\') {
			search();
		}
	}

	console_set_color(0xFFFFFF);
	console_set_background_color(0x000000);
	console_clear();
	console_print("mess: ");
	console_print(fname);
	console_println(" / closed.");
	console_set_title("Boink Operating System");
	return;
}

void search() {
	clear_status_line();
	console_set_cursor(0, available_rows);
	console_set_color(0x008888);
	console_print("search: ");
	char search_term[50];
	console_input_nonl(search_term, sizeof(search_term));
	print_status_line(search_term);
	
	match_count = 0;
	search_term_len = strlen(search_term);
	
	if (search_term_len == 0 || !txt) {
		print_status_line("empty search");
		return;
	}

	uint32_t i = 0;
	while (txt[i] && match_count < MAX_MATCHES) {
		if (mem_cmp(&txt[i], search_term, search_term_len) == 0) {
			match_indices[match_count++] = i;
		}
		i++;
	}

	console_clear();
	
	print_file_contents_at_offset();

	if (match_count > 0) {
		print_status_line("match(es) found");
	} else {
		print_status_line("no matches");
	}
}

void print_status_line(const char *msg) {
	clear_status_line();
	console_set_cursor(0, available_rows);
	console_set_color(0x9109ff);
	console_print("mess: ");
	console_print(msg);

	console_print(" | line ");
	console_print_dec(scroll_offset + 1);
	console_print("/");
	console_print_dec(count_visual_lines());
	console_print(" | q to quit");
}

void clear_status_line() {
	console_set_cursor(0, available_rows);
	uint32_t c = available_cols-1;
	while (c) {
		console_putc(' ');
		c--;
	}
}

void print_file_contents_at_offset() {
	if (!txt) return;
	
	console_clear();

	console_set_color(0x444444);

	uint32_t visual_lines_skipped = 0;
	uint32_t cursor_y = 0;
	uint32_t logical_line = 0;
	uint32_t current_txt_index = 0;

	char* ptr = txt;
	uint32_t col = LINE_NUM_WIDTH;

	while (*ptr && visual_lines_skipped < scroll_offset) {
		char c = *ptr++;

		if (c == '\n') {
			logical_line++;
			visual_lines_skipped++;
			col = LINE_NUM_WIDTH;
		} else if (c == '\t') {
			col += TAB_WIDTH - (col % TAB_WIDTH);
		} else {
			col++;
		}
		
		current_txt_index++;

		if (col >= available_cols) {
			visual_lines_skipped++;
			col = LINE_NUM_WIDTH;
		}
	}

	uint32_t cursor_x = LINE_NUM_WIDTH;
	bool new_visual_line = true;

	while (*ptr && cursor_y < available_rows - 1) {
		char c = *ptr;

		// start of visual line, print line number
		if (new_visual_line) {
			console_set_cursor(0, cursor_y);
			console_set_color(0xAAAAAA);

			char* numstr = itoa(logical_line + 1, 10);
			uint32_t len = strlen(numstr);

			console_print(numstr);
			for (uint32_t i = 0; i < LINE_NUM_WIDTH - 1 - len; ++i)
				console_print(" ");
			console_print("|");

			console_set_color(0x444444);
			cursor_x = LINE_NUM_WIDTH;
			new_visual_line = false;
		}

		console_set_cursor(cursor_x, cursor_y);

		if (c == '\n') {
			ptr++;
			current_txt_index++;
			cursor_y++;
			cursor_x = LINE_NUM_WIDTH;
			logical_line++;
			new_visual_line = true;
			continue;
		}

		if (c == '\t') {
			uint32_t spaces = TAB_WIDTH - (cursor_x % TAB_WIDTH);
			for (uint32_t i = 0; i < spaces && cursor_x < available_cols; i++) {
				console_set_cursor(cursor_x++, cursor_y);
				console_putc(' ');
			}
			ptr++;
			current_txt_index++;
			continue;
		}

		
		bool highlighted = false;
		for (uint32_t i = 0; i < match_count; i++) {
			if (match_indices[i] == current_txt_index) {
				highlighted = true;
				break;
			}

			if (match_indices[i] > current_txt_index)
				break;
		}

		if (highlighted && cursor_x + search_term_len < available_cols) {
			console_set_color(0x000000);
			console_set_background_color(0xFFFF00);

			for (uint32_t j = 0; j < search_term_len; j++) {
				console_set_cursor(cursor_x++, cursor_y);
				console_putc(txt[current_txt_index + j]);
			}

			console_set_background_color(0xFFFFFF); // reset bg
			console_set_color(0x444444); // reset fg

			current_txt_index += search_term_len;
			ptr += search_term_len;

			if (cursor_x >= available_cols) {
				cursor_y++;
				cursor_x = LINE_NUM_WIDTH;
				new_visual_line = true;
			}
			continue;
		}

		// normal char
		if (cursor_x < available_cols) {
			console_putc(c);
			cursor_x++;
		}
		ptr++;
		current_txt_index++;
		
		

		if (cursor_x >= available_cols) {
			cursor_y++;
			cursor_x = LINE_NUM_WIDTH;
			new_visual_line = true;
		}
	}
}

uint32_t count_visual_lines() {
	if (!txt) return 0;

	uint32_t vis_lines = 0;
	uint32_t col = 0;
	char* ptr = txt;

	uint32_t text_width = (available_cols > LINE_NUM_WIDTH) 
		? available_cols - LINE_NUM_WIDTH
		: 1; // safety: don't allow zero or negative

	while (*ptr) {
		char c = *ptr++;

		if (c == '\n') {
			vis_lines++;
			col = 0;
		} else if (c == '\t') {
			col += TAB_WIDTH - (col % TAB_WIDTH);
		} else {
			col++;
		}

		if (col >= text_width) {
			vis_lines++;
			col = 0;
		}
	}

	if (col > 0)
		vis_lines++;

	return vis_lines;
}

void scroll_down() {
	uint32_t max_scroll = count_visual_lines() - (available_rows - 1);

	if (scroll_offset < max_scroll) {
		scroll_offset++;
		print_file_contents_at_offset();
		print_status_line("scroll down");
	} else {
		print_status_line("bottom of file");
	}
}

void scroll_up() {
	if (scroll_offset > 0) {
		scroll_offset--;
		print_file_contents_at_offset();
		print_status_line("scroll up");
	} else {
		print_status_line("top of file");
	}
}