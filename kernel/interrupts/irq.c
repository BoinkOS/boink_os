#include "irq.h"
#include "idt.h"
#include "../drivers/video/vga_text.h"
#include "pic.h"

#define MAX_IRQ_HANDLERS 16
static irq_handler_t irq_handlers[MAX_IRQ_HANDLERS] = { 0 };

extern void irq0_handler(void);
extern void irq1_handler(void);
extern void irq2_handler(void);
extern void irq3_handler(void);
extern void irq4_handler(void);
extern void irq5_handler(void);
extern void irq6_handler(void);
extern void irq7_handler(void);
extern void irq8_handler(void);
extern void irq9_handler(void);
extern void irq10_handler(void);
extern void irq11_handler(void);
extern void irq12_handler(void);
extern void irq13_handler(void);
extern void irq14_handler(void);
extern void irq15_handler(void);

void irq_init(uint16_t code_selector) {
	install_ir_handler(32 + 0, FLAGS, code_selector, irq0_handler);
	install_ir_handler(32 + 1, FLAGS, code_selector, irq1_handler);
	install_ir_handler(32 + 2, FLAGS, code_selector, irq2_handler);
	install_ir_handler(32 + 3, FLAGS, code_selector, irq3_handler);
	install_ir_handler(32 + 4, FLAGS, code_selector, irq4_handler);
	install_ir_handler(32 + 5, FLAGS, code_selector, irq5_handler);
	install_ir_handler(32 + 6, FLAGS, code_selector, irq6_handler);
	install_ir_handler(32 + 7, FLAGS, code_selector, irq7_handler);
	install_ir_handler(32 + 8, FLAGS, code_selector, irq8_handler);
	install_ir_handler(32 + 9, FLAGS, code_selector, irq9_handler);
	install_ir_handler(32 +10, FLAGS, code_selector, irq10_handler);
	install_ir_handler(32 +11, FLAGS, code_selector, irq11_handler);
	install_ir_handler(32 +12, FLAGS, code_selector, irq12_handler);
	install_ir_handler(32 +13, FLAGS, code_selector, irq13_handler);
	install_ir_handler(32 +14, FLAGS, code_selector, irq14_handler);
	install_ir_handler(32 +15, FLAGS, code_selector, irq15_handler);
}

void irq_common_c(uint32_t irq_num) {
	if (irq_num < MAX_IRQ_HANDLERS && irq_handlers[irq_num]) {
		irq_handlers[irq_num](irq_num);  // call registered handler
	} else {
		vga_println("IRQ fired; handled by default handler. Unadvisable to not have IRQ handler.");
	}
	
	end_of_interrupt();
}

void irq_set_handler(uint8_t irq, irq_handler_t handler) {
	if (irq < MAX_IRQ_HANDLERS) {
		irq_handlers[irq] = handler;
	}
}