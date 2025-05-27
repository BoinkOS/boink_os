; ---------------------------------------------
; GDT STRUCTURE (3 entries): null, code, data
gdt_start:
     dq 0x0000000000000000 ; null descriptor
     dq 0x00CF9A000000FFFF ; code segment: base=0, limit=4GB
     dq 0x00CF92000000FFFF ; data segment: base=0, limit=4GB
gdt_end:

gdt_descriptor:
     dw gdt_end - gdt_start - 1 ; size (limit)
     dd gdt_start ; address

; ---------------------------------------------
load_gdt:
     lgdt [gdt_descriptor]
     ret