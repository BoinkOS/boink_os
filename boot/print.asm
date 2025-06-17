[bits 16]
print:
	; input: SI points to null-terminated string
.print_loop:
	lodsb ; load byte at [SI] into AL, advance SI
	cmp al, 0
	je .newline ; jump to newline part if end of string
	mov ah, 0x0E
	int 0x10
	jmp .print_loop

.newline:
	mov al, 0x0D ; carriage return '\r'
	mov ah, 0x0E
	int 0x10
	mov al, 0x0A ; line feed '\n'
	int 0x10
	ret