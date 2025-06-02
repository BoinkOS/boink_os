section .text
extern irq_common_c            ; C entry point (takes IRQ number)

%macro IRQ_STUB 1
global irq%1_handler
irq%1_handler:
    cli
    pushad                     ; save registers (32 bytes)
    push dword %1              ; push IRQ number **after** pushad
    call irq_common_c
    add esp, 4                 ; pop argument

    popad
    ; send EOI
    mov al, 0x20
    out 0x20, al
    %if %1 >= 8
        out 0xA0, al
    %endif
    iret
%endmacro

IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15