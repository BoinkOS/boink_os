; syscall_entry.asm
global syscall_entry

section .text
syscall_entry:
	; save registers if needed
	pusha

	; call C handler
	extern syscall_handler
	call syscall_handler
	mov [esp + 28], eax
	
	popa
	iret