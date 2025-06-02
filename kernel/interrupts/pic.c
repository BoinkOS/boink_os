// pic.c
#include "pic.h"
#include "../llio.h"

void pic_remap(void) {
	pbout(PIC1_COMMAND, 0x11);
	pbout(PIC2_COMMAND, 0x11);
	pbout(PIC1_DATA, 0x20); // master offset
	pbout(PIC2_DATA, 0x28); // slave offset
	pbout(PIC1_DATA, 0x04);
	pbout(PIC2_DATA, 0x02);
	pbout(PIC1_DATA, 0x01);
	pbout(PIC2_DATA, 0x01);
	pbout(PIC1_DATA, 0x0);
	pbout(PIC2_DATA, 0x0);
}

void send_eoi(uint8_t irq) {
	if (irq >= 8)
	    pbout(PIC2_COMMAND, PIC_EOI);
	pbout(PIC1_COMMAND, PIC_EOI);
}

void end_of_interrupt() {
	pbout(PIC2_COMMAND, PIC_EOI);
	pbout(PIC1_COMMAND, PIC_EOI);
}