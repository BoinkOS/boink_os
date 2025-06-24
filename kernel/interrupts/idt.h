#pragma once
#include <stdint.h>

#define MAX_INTERRUPTS 256
#define FLAGS 0x8E

#define MAX_EXCEPTION_HANDLERS 32

typedef int (*exception_handler_t)(uint32_t interrupt_number, uint32_t error_code);

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

typedef struct {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_num, err_code;
	uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

typedef void (*IRQ_HNDLR)(void);

void idt_init(uint16_t code_selector);
int install_ir_handler(uint32_t i, uint8_t flags, uint16_t selector, IRQ_HNDLR irq);
void idt_install(void);
void exception_common_c(registers_t* regs);
void set_exception_handler(uint8_t int_num, exception_handler_t handler);
void dump_registers(registers_t* regs);
void extensible_exception_handler(registers_t* regs);
void rudimentry_exception_logger(registers_t* regs);