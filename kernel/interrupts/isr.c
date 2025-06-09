// isr.c
#include "isr.h"
#include "../drivers/video/vga_text.h"
#include "../llio.h"
#include "pic.h"

void idt_default_handler() {
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");


	set_attribute_byte(0x4F);
	vga_println("!!! PANIC !!!");
	vga_println("An exception has occurred and has been handled by the kernel's default handler.");
	vga_println("");
	vga_println("It is advisable to implement your own exception handling.");


    // halt CPU
	for (;;) __asm__ __volatile__("hlt");

	__asm__ __volatile__("popa; leave; iret");
}