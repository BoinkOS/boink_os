[bits 16]
[org 0x7c00]

start:
     cli
	xor ax, ax	; zero out
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, STACK	; set stack address
	mov sp, bp
	
     mov si, entry_msg
     call print
	
	call load_kernel
	
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

; constants
KERNEL_OFFSET equ 0x1000
STACK equ 0x9000

; variables
BOOT_DRV db 0

; includes
%include "boot/print.asm"
%include "boot/gdt.asm"
%include "boot/load_sec.asm"
%include "boot/load_kern.asm"
%include "boot/pm.asm"
; %include "boot/kern_entry.asm"

; ---

; MBR boot signature
times 510-($-$$) db 0 ; padding
dw 0xaa55 ; magic number