#include "keymap.h"

const char scancode_to_ascii[128] = {
	0,  27, '1', '2', '3', '4', '5', '6',  // 0x00–0x07
	'7', '8', '9', '0', '-', '=', '\b',   // Backspace
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u',
	'i', 'o', 'p', '[', ']', '\n', 0,     // Enter, Ctrl
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k',
	'l', ';', '\'', '`', 0, '\\', 'z', 'x',
	'c', 'v', 'b', 'n', 'm', ',', '.', '/',
	0,   '*', 0, ' ', 0, 0, 0, 0,         // Spacebar
    /* rest can be 0 or extended scancodes */
};
