[bits 16]
[org 0x7c00]

start:
     mov si, message
	call print

hang:
     jmp hang ; inf loop so we don't crash

; ---

print:
    ; input: SI points to null-terminated string
.print_loop:
    lodsb ; loads byte at [SI] into AL, advances SI
    cmp al, 0
    je .done
    mov ah, 0x0E
    int 0x10
    jmp .print_loop
.done:
    ret


message db "Hello from boink bootloader!", 0

; ---

; MBR boot signature
times 510-($-$$) db 0 ; padding
dw 0xaa55 ; magic number