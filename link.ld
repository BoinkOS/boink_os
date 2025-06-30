ENTRY(kmain)

SECTIONS {
	. = 0x00100000;

	.text : {
		KEEP(*(.text.boot))
		*(.text)
	}

	.rodata : {
		*(.rodata*)
	}

	.data : {
		*(.data)
	}

	.bss : {
		*(.bss)
		*(COMMON)
	}
}