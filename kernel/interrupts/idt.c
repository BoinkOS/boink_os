#include "idt.h"
#include "../llio.h"

static idt_desc _idt[MAX_INTERRUPTS];
static idtr _idtr;

void idt_install() {
    __asm__ __volatile__("lidt %0" : : "m" (_idtr));
}

int install_ir_handler(uint32_t i, uint8_t flags, uint16_t selector, IRQ_HNDLR irq) {
    if (i >= MAX_INTERRUPTS || !irq) return 0;

    uint32_t addr = (uint32_t) irq;
    _idt[i].base_low = addr & 0xFFFF;
    _idt[i].base_high = (addr >> 16) & 0xFFFF;
    _idt[i].selector = selector;
    _idt[i].reserved = 0;
    _idt[i].flags = flags;

    return 1;
}

void idt_init(uint16_t code_selector) {
    _idtr.limit = sizeof(idt_desc) * MAX_INTERRUPTS - 1;
    _idtr.base = (uint32_t)&_idt[0];

    // fill 0–31 (exceptions) with default
    extern void idt_default_handler(void);
    for (int i = 0; i < 32; ++i) {
        install_ir_handler(i, FLAGS, code_selector, idt_default_handler);
    }

    idt_install();
}
