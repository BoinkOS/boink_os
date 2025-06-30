; ask BIOS for VESA controller info
vesa_init:
	mov ax, 0x4F00
	mov di, VESA_INFO_BLOCK
	int 0x10
	cmp ax, 0x004F
	jne vesa_fail ; if not supported, panic

	; set VESA mode
	mov ax, 0x4F02
	mov bx, VESA_MODE | VESA_LFB ; mode + linear framebuffer flag
	int 0x10
	cmp ax, 0x004F
	jne vesa_fail ; if setting mode failed, panic

	; get mode info
	mov ax, 0x4F01
	mov cx, VESA_MODE
	mov di, VESA_MODE_INFO_BLOCK
	int 0x10
	cmp ax, 0x004F
	jne vesa_fail ; couldn't get mode info, panic

	mov ax, [VESA_MODE_INFO_BLOCK + 0x28] ; lower 16 bits
	mov dx, [VESA_MODE_INFO_BLOCK + 0x2A] ; upper 16 bits

	movzx eax, ax    ; zero-extend lower 16 bits into eax
	shl edx, 16      ; shift upper 16 bits to high part
	or eax, edx      ; combine into full 32-bit address

	mov [FRAMEBUFFER_ADDR], eax ; store full address
	
	; pitch
	mov ax, [VESA_MODE_INFO_BLOCK + 0x10]
	mov [FRAMEBUFFER_PITCH], ax

	; width
	mov ax, [VESA_MODE_INFO_BLOCK + 0x12]
	mov [FRAMEBUFFER_WIDTH], ax

	; height
	mov ax, [VESA_MODE_INFO_BLOCK + 0x14]
	mov [FRAMEBUFFER_HEIGHT], ax

	; bpp
	mov al, [VESA_MODE_INFO_BLOCK + 0x19]
	mov [FRAMEBUFFER_BPP], al

	ret

vesa_fail:
	mov si, vesa_fail_msg
	call print
	hlt
	jmp $
	
vesa_fail_msg db "vesa init failed!", 0