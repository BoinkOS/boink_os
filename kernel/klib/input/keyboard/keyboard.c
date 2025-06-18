#include "keyboard.h"
#include "keymap.h"
#include "buffer.h"
#include "../../../interrupts/irq.h"
#include "../../../llio.h"

#define KEYBOARD_IRQ 1
#define DATA_PORT 0x60

static int shift = 0;
static int ctrl = 0;
static int alt = 0;
static int caps = 0;

void keyboard_handler(uint32_t irq_num) {
	(void)irq_num;
	uint8_t scancode = pbin(DATA_PORT); // read scancode

	// key release
	if (scancode & 0x80) {
		uint8_t released = scancode & 0x7F;
		if (released == SC_LSHIFT || released == SC_RSHIFT) shift = 0;
		else if (released == SC_CTRL) ctrl = 0;
		else if (released == SC_ALT) alt = 0;
		return;
	}

	// key press
	switch (scancode) {
		case SC_LSHIFT: case SC_RSHIFT: shift = 1; return;
		case SC_CTRL: ctrl = 1; return;
		case SC_ALT: alt = 1; return;
		case SC_CAPSLOCK: caps = !caps; return;
		case SC_BACKSPACE:
			kb_buffer_put('\b'); return;
		case SC_ENTER:
			kb_buffer_put('\n'); return;
	}

	char c = shift ? keymap_shift[scancode] : keymap_normal[scancode];

	// handle caps lock for alphabet
	if (caps && c >= 'a' && c <= 'z') {
		c -= 32;
	} else if (caps && c >= 'A' && c <= 'Z' && !shift) {
		c += 32;
	}

	if (c) {
		kb_buffer_put(c);
	}
}

void keyboard_init(void) {
	kb_buffer_init();
	irq_set_handler(1, keyboard_handler);
}

char kbd_read_char(void) {
	return kb_buffer_get();
}

int kbd_has_char(void) {
	return kb_buffer_has_char();
}