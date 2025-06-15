#include "drivers/video/vga_text.h"
#include "interrupts/idt.h"
#include "interrupts/pic.h"
#include "interrupts/irq.h"
#include "interrupts/pit.h"
#include "klib/uptime/uptime.h"
#include "klib/input/keyboard/keyboard.h"
#include "disk/disk.h"
#include "fs/glfs.h"
extern void pit_uptime_handler(uint32_t irq);
extern void ata_irq_handler(uint32_t irq_num);

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
	
	
	vga_println("setting IRQ0 handler for uptime...");
	irq_set_handler(0, pit_uptime_handler);
	keyboard_init();
	
	vga_println("maskable interrupts will be enabled after next instruction.");
	__asm__ __volatile__("sti");
	
	// while (1);  // freeze so the CPU doesn’t start interpreting RAM



	
	vga_println("initializing disk...");

	irq_set_handler(14, ata_irq_handler);
	disk_init();
	vga_println("reading sector 0...");
	uint8_t buf[512];
	disk_read(0, buf);
	vga_println("done reading sector 0. attempting to read GLFS superblock...");
	
	char id[9];
	for (int i = 0; i < 8; i++)
		id[i] = buf[i];
	id[8] = '\0';
	
	int is_glfs_verified = check_glfs_magic(buf);
	
	if (is_glfs_verified) {
		vga_print("read superblock identifier: ");
		set_attribute_byte(0x02);
		vga_print(id);
		set_attribute_byte(0x0F);
		vga_println("GLFS READ CONFIRMED!!!");
	} else {
		set_attribute_byte(0x4F);
		vga_println("Unable to verify primary slave as GLFS disk.");
		while (1) ;
	}
	

	while (1) {
		if (kbd_has_char()) {
			char c = kbd_read_char();
			
			vga_printchar(c);  // or your own print fn
		}
	}
}