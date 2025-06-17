ENTRY(kmain)

SECTIONS {
	. = 0x1000;  /* load address in RAM */

	.text : {
		*(.text)
	}

	.data : {
		*(.data)
	}

	.bss : {
		*(.bss COMMON)
		*(COMMON)
	}
}