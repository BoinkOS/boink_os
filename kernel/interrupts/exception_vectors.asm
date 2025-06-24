[bits 32]

section .text
extern exception_common_c

%macro ISR_NOERR 1
global isr%1_handler
isr%1_handler:
	cli
	push dword 0         ; fake error code
	push dword %1        ; interrupt number
	jmp isr_common_stub
%endmacro

%macro ISR_ERR 1
global isr%1_handler
isr%1_handler:
	cli
	push dword %1        ; interrupt number (error code is already on stack)
	jmp isr_common_stub
%endmacro

isr_common_stub:
	; push general-purpose registers
	pusha

	; segment registers
	push ds
	push es
	push fs
	push gs

	; set segments
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; load current ESP as pointer to registers struct
	mov eax, esp
	push eax

	call exception_common_c
	add esp, 4  ; clean up pushed ptr

	; restore segs and GPRs
	pop gs
	pop fs
	pop es
	pop ds
	popa

	add esp, 8  ; clean up int_num + error_code

	sti
	iret

ISR_NOERR 0   ; Divide by zero
ISR_NOERR 1   ; Debug
ISR_NOERR 2   ; NMI
ISR_NOERR 3   ; Breakpoint
ISR_NOERR 4   ; Overflow
ISR_NOERR 5   ; Bound Range Exceeded
ISR_NOERR 6   ; Invalid Opcode
ISR_NOERR 7   ; Device Not Available
ISR_ERR   8   ; Double Fault
ISR_NOERR 9   ; Coprocessor Segment Overrun (legacy)
ISR_ERR   10  ; Invalid TSS
ISR_ERR   11  ; Segment Not Present
ISR_ERR   12  ; Stack Segment Fault
ISR_ERR   13  ; General Protection Fault
ISR_ERR   14  ; Page Fault
ISR_NOERR 15  ; Reserved
ISR_NOERR 16  ; x87 Floating Point Exception
ISR_ERR   17  ; Alignment Check
ISR_NOERR 18  ; Machine Check
ISR_NOERR 19  ; SIMD Floating-Point Exception
ISR_NOERR 20  ; Virtualization Exception
ISR_ERR   21  ; Control Protection Exception
ISR_NOERR 22  ; Reserved
ISR_NOERR 23  ; Reserved
ISR_NOERR 24  ; Reserved
ISR_NOERR 25  ; Reserved
ISR_NOERR 26  ; Reserved
ISR_NOERR 27  ; Reserved
ISR_NOERR 28  ; Hypervisor Injection Exception
ISR_NOERR 29  ; VMM Communication Exception
ISR_NOERR 30  ; Security Exception
ISR_NOERR 31  ; Reserved