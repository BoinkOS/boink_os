; user_switch.asm
global switch_to_user_mode
switch_to_user_mode:
	cli

	; get user_stack and entry point before touching stack
	mov ecx, [esp + 4]     ; user_entry
	mov eax, [esp + 8]     ; user_stack

	mov ax, 0x23           ; user data selector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push 0x23              ; ss
	push eax               ; esp
	pushf
	push 0x1B              ; cs
	push ecx               ; eip

	iret