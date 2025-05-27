[bits 16]
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