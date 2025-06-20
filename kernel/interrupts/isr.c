// isr.c
#include "isr.h"
#include "../klib/console/console.h"
#include "../llio.h"
#include "pic.h"

void idt_default_handler() {
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	console_set_background_color(0xFF0000);
	console_set_color(0xFFFFFF);
	console_println("!!! PANIC !!!");
	console_println("An exception has occurred and has been handled by the kernel's default handler.");
	console_println("");
	console_println("It is advisable to implement your own exception handling.");


	// halt CPU
	for (;;) __asm__ __volatile__("hlt");

	__asm__ __volatile__("popa; leave; iret");
}