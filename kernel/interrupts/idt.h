#pragma once
#include <stdint.h>

#define MAX_INTERRUPTS 256
#define FLAGS 0x8E

typedef struct {
    uint16_t base_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_desc;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr;

typedef void (*IRQ_HNDLR)(void);

void idt_init(uint16_t code_selector);
int install_ir_handler(uint32_t i, uint8_t flags, uint16_t selector, IRQ_HNDLR irq);
void idt_install(void);
