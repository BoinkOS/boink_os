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
     mov ax, 0x10        ; data segment selector
     mov ds, ax
     mov es, ax
     mov fs, ax
     mov gs, ax
     mov ss, ax

     ; do something in 32-bit mode to prove it
     mov dword [0xB8000], 0x2F4B   ; write 'K' to screen with color

.hang:
     jmp .hang