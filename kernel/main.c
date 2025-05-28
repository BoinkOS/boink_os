#include "utils.h"


void kmain(void) {
	set_text_color(0x1F);
	println("--- BoinkOS Kernel --------------------------------------------");
	println("--- ASCII Test Seq --------------------------------------------");
	
	set_text_color(0x2F);
	println("  ! \" # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?");
	println("@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \\ ] ^ _");
	println("` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~  ");
	
	while (1);  // freeze so the CPU doesn’t start interpreting RAM
}