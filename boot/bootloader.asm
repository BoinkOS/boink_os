[bits 16]
[org 0x7c00]

start:
     cli
	
     mov si, entry_msg
     call print
	
     mov si, gdt_msg
     call print
     call load_gdt
	
     mov si, pm_msg
     call print
     call enter_pm

; ---

entry_msg db "hello from boink bootloader!", 0
gdt_msg db "loading gdt...", 0
pm_msg db "entering protected mode...", 0

%include "boot/print.asm"
%include "boot/gdt.asm"
%include "boot/pm.asm"

; ---

; MBR boot signature
times 510-($-$$) db 0 ; padding
dw 0xaa55 ; magic number