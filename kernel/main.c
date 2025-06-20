// #include "drivers/vga_text/vga_text.h"
#include "interrupts/idt.h"
#include "interrupts/pic.h"
#include "interrupts/irq.h"
#include "interrupts/pit.h"
#include "klib/uptime/uptime.h"
#include "klib/input/keyboard/keyboard.h"
#include "disk/disk.h"
#include "fs/glfs.h"
#include "main.h"
#include "drivers/video/fb.h"
#include "drivers/video/text.h"
#include "klib/console/console.h"
#include "utils.h"
extern void pit_uptime_handler(uint32_t irq);
extern void ata_irq_handler(uint32_t irq_num);

void kmain(void) {
	init_framebuffer();

	console_init();

	console_set_color(0x888888);
	console_println("vesa mode entered, gdt loaded, kernel loaded, protected mode entered prior to jumping to kernel.\n");
	console_set_color(0xFFFFFF);
	
	console_println("hello from boink kernel!");
	
	console_println("remapping PIC...");
	pic_remap();
	
	console_println("initialising IDT...");
	idt_init(0x08);
	
	console_println("initialising IRQs...");
	irq_init(0x08);
	
	console_println("initialising programmable interval timer...");
	pit_init(100);
	
	console_println("setting IRQ0 handler for uptime...");
	irq_set_handler(0, pit_uptime_handler);
	
	console_println("initialising keyboard...");
	keyboard_init();
	
	console_println("maskable interrupts will be enabled after next instruction.");
	__asm__ __volatile__("sti");
	
	console_println("initializing disk...");
	irq_set_handler(14, ata_irq_handler);
	disk_init();
	console_println("reading sector 0...");
	uint8_t buf[512];
	disk_read(0, buf);
	console_println("done reading sector 0. attempting to read GLFS superblock...");
	
	char id[9];
	for (int i = 0; i < 8; i++)
		id[i] = buf[i];
	id[8] = '\0';
	
	int is_glfs_verified = check_glfs_magic(buf);
	
	if (is_glfs_verified) {
		console_print("read superblock identifier: ");
		console_set_color(0x00FF00);
		console_print(id);
		console_set_color(0xFFFFFF);
		console_print("[ ");
		console_set_color(0x00FF00);
		console_print("GLFS PASS");
		console_set_color(0xFFFFFF);
		console_print(" ]");
		console_println("");
	} else {
		console_set_background_color(0xFF0000);
		console_set_color(0xFFFFFF);
		console_println("Unable to verify primary slave as GLFS disk.");
		while (1) ;
	}
	
	glfs_read_directory();
	glfs_list_files();
	//glfs_file_loader();
	
	console_set_color(0xFFFFFF);
	console_set_background_color(0x0000FF);
	console_println("--- BoinkOS Kernel --------------------------------------------");
	console_println("--- ASCII Test Seq --------------------------------------------");
	console_set_background_color(0x00AA00);
	console_println("  ! \" # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?");
	console_println("@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \\ ] ^ _");
	console_println("` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~  ");
	console_set_background_color(0x000000);
	console_set_color(0xFFFFFF);
	console_println("");
	console_print("OK to start console? (y/n) ");
	
	char conf = read_key();
	console_set_color(0x9019ff);
	console_putc(conf);
	console_set_color(0xFFFFFF);
	console_putc('\n');
	if (conf != 'Y' && conf != 'y') {
		console_set_background_color(0xFF0000);
		console_set_color(0xFFFFFF);
		console_println("Console aborted. Restart system.");
		while (1);
	} else {
		console_set_background_color(0x9019ff);
		console_println("\n\n~~~ Welcome to BoinkOS! ~~~                  ");
		console_set_color(0xc98fff);
		console_println("~ where there is a shell, there is a way... ~\n\n");
		console_set_background_color(0x000000);
		console_set_color(0xFFFFFF);
	}
	
	char input[128];
	while (1) {
		console_set_color(0xFFFF00);
		console_print("boink$ ");
		console_set_color(0xFFFFFF);
		console_input(input, sizeof(input));
		console_print("ECHO: ");
		console_print(input);
		console_putc('\n');
		console_putc('\n');
	}
}