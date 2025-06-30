global gdt_size
gdt_size dw gdt_end - gdt_start
global load_gdt
global gdt
gdt: ;
gdt_start:
	dq 0x0000000000000000 ; null descriptor
	dq 0x00CF9A000000FFFF ; code segment: base=0, limit=4GB
	dq 0x00CF92000000FFFF ; data segment: base=0, limit=4GB
	dq 0x00CFFA000000FFFF ; user code (ring 3)
	dq 0x00CFF2000000FFFF ; user data (ring 3)
	times 1 dq 0 ; placeholder for TSS (index 5)
gdt_end:

gdt_descriptor:
	dw gdt_end - gdt_start - 1 ; size (limit)
	dd gdt_start ; address

; ---------------------------------------------
load_gdt:
	lgdt [gdt_descriptor]
	ret