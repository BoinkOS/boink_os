[bits 16]
[org 0x7c00]

start:
     mov si, message
	call print

hang:
     jmp hang ; inf loop so we don't crash

; ---

%include "boot/print.asm"


message db "Hello from boink bootloader!", 0

; ---

; MBR boot signature
times 510-($-$$) db 0 ; padding
dw 0xaa55 ; magic number