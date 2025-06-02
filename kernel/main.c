#include "drivers/video/vga_text.h"
#include "interrupts/idt.h"
#include "interrupts/pic.h"
#include "interrupts/irq.h"
#include "interrupts/pit.h"


void kmain(void) {
	//clear_screen();
	set_attribute_byte(0x1F); // white on blue
	vga_println("--- BoinkOS Kernel --------------------------------------------");
	vga_println("--- ASCII Test Seq --------------------------------------------");
	
	set_attribute_byte(0x2F);
	vga_println("  ! \" # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?");
	vga_println("@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \\ ] ^ _");
	vga_println("` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~  ");
	
	
	set_attribute_byte(0x0f);

	vga_println("remapping PIC...");
	pic_remap();
	
	vga_println("initialising IDT...");
	idt_init(0x08);
	
	vga_println("initialising IRQs...");
	irq_init(0x08);
	
	vga_println("initialising programmable interval timer...");
	pit_init(100);
	
	__asm__ __volatile__("sti");
	vga_println("maskable interrupts now enabled.");
	
	while (1);  // freeze so the CPU doesn’t start interpreting RAM
}