#include "keyboard.h"
#include "keymap.h"
#include "buffer.h"
#include "../../../interrupts/irq.h"
#include "../../../llio.h"

#define KEYBOARD_IRQ 1
#define DATA_PORT 0x60

void keyboard_handler(uint32_t irq_num) {
    uint8_t scancode = pbin(DATA_PORT); // read scancode

    if (scancode & 0x80) {
        // ignore key releases for now
        return;
    }

    char c = scancode_to_ascii[scancode];
    if (c) {
        kb_buffer_put(c); // put it in the buffer
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