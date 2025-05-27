[BITS 16]
[ORG 0x7C00]

start:
     mov ah, 0x0E             ; enter teletype mode
     mov al, 'H'
     int 0x10
     mov al, 'i'
     int 0x10
     mov al, '!'
     int 0x10

hang:
     jmp hang                 ; inf loop so we don't crash


times 510 - ($ - $$) db 0     ; pad to 512b to fit boot sector
dw 0xAA55                     ; boot signature to mark as bootable