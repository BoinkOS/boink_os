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
	
	mov si, vesa_msg
	call print
	call vesa_init
	
	mov si, gdt_msg
	; call vesa_print       ; (*)
	call load_gdt
	
	mov si, kernel_load_msg
	; call vesa_print       ; (*)
	call load_kernel
	
	mov si, pm_msg
	; call vesa_print       ; (*)
	jmp enter_pm

; constants
KERNEL_OFFSET equ 0x1000
STACK equ 0x7000
VESA_MODE equ 0x118
VESA_LFB equ 0x4000 ; linear framebuffer flag
VESA_INFO_BLOCK equ 0x8000 
VESA_MODE_INFO_BLOCK equ 0x9000
FRAMEBUFFER_ADDR equ 0x7E00
FRAMEBUFFER_PITCH equ 0x7E04
FRAMEBUFFER_WIDTH equ 0x7E06
FRAMEBUFFER_HEIGHT equ 0x7E08
FRAMEBUFFER_BPP equ 0x7E0A
vesa_msg db "setting video mode...", 0

; variables
BOOT_DRV db 0
entry_msg db "hello from boink bootloader!", 0
kernel_load_msg db "loading kernel...", 0
gdt_msg db "loading gdt...", 0
pm_msg db "entering protected mode...", 0

; includes
%include "boot/print.asm"
%include "boot/gdt.asm"
%include "boot/load_sec.asm"
%include "boot/load_kern.asm"
%include "boot/vesa.asm"
%include "boot/pm.asm"

; ---
; (*) see... i don't really need this now. using a text mode print function in vesa graphics mode results in immediate crashes and bootloops.

; MBR boot signature
times 510-($-$$) db 0 ; padding
dw 0xaa55 ; magic number