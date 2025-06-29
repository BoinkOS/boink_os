[bits 32]

; user_switch.asm
global switch_to_user_mode
switch_to_user_mode:
	cli
	
	; get entry + stack ptr safely
	mov ecx, [esp + 4] ; entry
	mov eax, [esp + 8] ; stack
	
	mov dx, 0x23
	mov ds, dx
	mov es, dx
	mov fs, dx
	mov gs, dx
	
	push 0x23
	push eax
	pushf
	push 0x1B
	push ecx
	
	iret