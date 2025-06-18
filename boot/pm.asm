enter_pm:
	; set PE bit in CR0 to enable protected mode
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	; far jump to flush pipeline + set CS to 32-bit code segment (selector 0x08)
	jmp 0x08:protected_mode

; ---------------------------------------------
[bits 32]

protected_mode:
	; set up segments
	mov ax, 0x10	   ; data segment selector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	mov esp, 0x9000
	
	call 0x1000

.hang:
	jmp .hang