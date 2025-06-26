#include "idt.h"
#include "../llio.h"
#include "../mem/paging.h"
#include "../klib/console/console.h"

extern void isr0_handler(); 
extern void isr1_handler(); 
extern void isr2_handler(); 
extern void isr3_handler(); 
extern void isr4_handler(); 
extern void isr5_handler(); 
extern void isr6_handler(); 
extern void isr7_handler(); 
extern void isr8_handler(); 
extern void isr9_handler(); 
extern void isr10_handler();
extern void isr11_handler();
extern void isr12_handler();
extern void isr13_handler();
extern void isr14_handler();
extern void isr15_handler();
extern void isr16_handler();
extern void isr17_handler();
extern void isr18_handler();
extern void isr19_handler();
extern void isr20_handler();
extern void isr21_handler();
extern void isr22_handler();
extern void isr23_handler();
extern void isr24_handler();
extern void isr25_handler();
extern void isr26_handler();
extern void isr27_handler();
extern void isr28_handler();
extern void isr29_handler();
extern void isr30_handler();
extern void isr31_handler();

extern void syscall_entry();




static idt_desc _idt[MAX_INTERRUPTS];
static idtr _idtr;

char error_names[32][40] = {"Divide by zero", "Debug", "NMI", "Breakpoint", "Overflow", "Bound Range Exceeded", "Invalid Opcode", "Device Not Available", "Double Fault", "Coprocessor Segment Overrun (legacy)", "Invalid TSS", "Segment Not Present", "Stack Segment Fault", "General Protection Fault", "Page Fault", "Reserved", "x87 Floating Point Exception", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception", "Virtualization Exception", "Control Protection Exception", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Hypervisor Injection Exception", "VMM Communication Exception", "Security Exception", "Reserved"};

static exception_handler_t exception_handlers[MAX_EXCEPTION_HANDLERS] = {0};

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
	
	install_ir_handler(0, FLAGS, code_selector, isr0_handler);
	install_ir_handler(1, FLAGS, code_selector, isr1_handler);
	install_ir_handler(2, FLAGS, code_selector, isr2_handler);
	install_ir_handler(3, FLAGS, code_selector, isr3_handler);
	install_ir_handler(4, FLAGS, code_selector, isr4_handler);
	install_ir_handler(5, FLAGS, code_selector, isr5_handler);
	install_ir_handler(6, FLAGS, code_selector, isr6_handler);
	install_ir_handler(7, FLAGS, code_selector, isr7_handler);
	install_ir_handler(8, FLAGS, code_selector, isr8_handler);
	install_ir_handler(9, FLAGS, code_selector, isr9_handler);
	install_ir_handler(10, FLAGS, code_selector, isr10_handler);
	install_ir_handler(11, FLAGS, code_selector, isr11_handler);
	install_ir_handler(12, FLAGS, code_selector, isr12_handler);
	install_ir_handler(13, FLAGS, code_selector, isr13_handler);
	install_ir_handler(14, FLAGS, code_selector, isr14_handler);
	install_ir_handler(15, FLAGS, code_selector, isr15_handler);
	install_ir_handler(16, FLAGS, code_selector, isr16_handler);
	install_ir_handler(17, FLAGS, code_selector, isr17_handler);
	install_ir_handler(18, FLAGS, code_selector, isr18_handler);
	install_ir_handler(19, FLAGS, code_selector, isr19_handler);
	install_ir_handler(20, FLAGS, code_selector, isr20_handler);
	install_ir_handler(21, FLAGS, code_selector, isr21_handler);
	install_ir_handler(22, FLAGS, code_selector, isr22_handler);
	install_ir_handler(23, FLAGS, code_selector, isr23_handler);
	install_ir_handler(24, FLAGS, code_selector, isr24_handler);
	install_ir_handler(25, FLAGS, code_selector, isr25_handler);
	install_ir_handler(26, FLAGS, code_selector, isr26_handler);
	install_ir_handler(27, FLAGS, code_selector, isr27_handler);
	install_ir_handler(28, FLAGS, code_selector, isr28_handler);
	install_ir_handler(29, FLAGS, code_selector, isr29_handler);
	install_ir_handler(30, FLAGS, code_selector, isr30_handler);
	install_ir_handler(31, FLAGS, code_selector, isr31_handler);
	
	install_ir_handler(0x80, 0xEE, 0x08, syscall_entry);

	idt_install();
}

void set_exception_handler(uint8_t int_num, exception_handler_t handler) {
	if (int_num < MAX_EXCEPTION_HANDLERS) {
		exception_handlers[int_num] = handler;
	}
}

void exception_common_c(registers_t* regs) {
	extensible_exception_handler(regs);
}

void extensible_exception_handler(registers_t* regs) {
	console_set_background_color(0xFF0000);
	console_set_color(0xFFFFFF);

	uint32_t int_num = regs->int_num;
	uint32_t error_code = regs->err_code;

	// call user-defined handler
	if (int_num < MAX_EXCEPTION_HANDLERS && exception_handlers[int_num]) {
		int result = exception_handlers[int_num](int_num, error_code);
		if (result == 1) {
			console_set_background_color(0x000000);
			console_set_color(0xFFFFFF);
			console_println("");
			return;
		}
	}

	console_println("");
	console_println("!!! KERNEL PANIC !!!");
	console_println("");
	console_print("Unhandled exception: ");
	console_print_dec(int_num);
	console_print(" (");
	console_print(error_names[int_num]);
	console_print(")");
	console_println("");
	console_print("Error code: 0x");
	console_print_hex(error_code);
	console_println("");

	// dump registers for debugging
	dump_registers(regs);

	console_println("The kernel is now in an unrecoverable state.");
	for (;;) __asm__ __volatile__("hlt");
}

void rudimentry_exception_logger(registers_t* regs) {
	console_set_background_color(0xFF0000);
	console_set_color(0xFFFFFF);

	console_print("EXCEPTION: ");
	console_print_dec(regs->int_num);
	console_print(" - ");
	console_print(error_names[regs->int_num]);
	console_println("");

	console_print("Error code: 0x");
	console_print_hex(regs->err_code);
	console_println("");

	console_print("EIP: ");
	console_print_hex(regs->eip);
	console_print(" | CS: ");
	console_print_hex(regs->cs);
	console_println("");

	dump_registers(regs);

	for (;;)
		__asm__ __volatile__("hlt");
}

void dump_registers(registers_t* regs) {
	console_println("Register dump:");
	console_print(" DS: "); console_print_hex(regs->ds); console_println("");
	console_print(" ES: "); console_print_hex(regs->es); console_println("");
	console_print(" FS: "); console_print_hex(regs->fs); console_println("");
	console_print(" GS: "); console_print_hex(regs->gs); console_println("");
	console_print("ESI: "); console_print_hex(regs->esi); console_println("");
	console_print("EDI: "); console_print_hex(regs->edi); console_println("");
	console_print("EBP: "); console_print_hex(regs->ebp); console_println("");
	console_print("ESP: "); console_print_hex(regs->esp); console_println("");
	console_print("EAX: "); console_print_hex(regs->eax); console_println("");
	console_print("EBX: "); console_print_hex(regs->ebx); console_println("");
	console_print("ECX: "); console_print_hex(regs->ecx); console_println("");
	console_print("EDX: "); console_print_hex(regs->edx); console_println("");
	console_println("------------------------------");
	console_print(" int_num: "); console_print_dec(regs->int_num);  console_println("");
	console_print("err_code: "); console_print_dec(regs->err_code); console_println("");
	console_println("------------------------------");
	console_print("    eip: "); console_print_hex(regs->eip); console_println("    ");
	console_print("     cs: "); console_print_hex(regs->cs); console_println("     ");
	console_print(" eflags: "); console_print_hex(regs->eflags); console_println(" ");
	console_print("useresp: "); console_print_hex(regs->useresp); console_println("");
	console_print("     ss: "); console_print_hex(regs->ss); console_println("     ");
	console_print(" clarus: moof.");                         console_println("     ");
}