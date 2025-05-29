#include "drivers/video/vga_text.h"


void kmain(void) {
	clear_screen();
	set_attribute_byte(0x1F); // white on blue
	vga_println("--- BoinkOS Kernel --------------------------------------------");
	vga_println("--- ASCII Test Seq --------------------------------------------");
	
	set_attribute_byte(0x2F);
	vga_println("  ! \" # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?");
	vga_println("@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \\ ] ^ _");
	vga_println("` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~  ");
	
	while (1);  // freeze so the CPU doesn’t start interpreting RAM
}