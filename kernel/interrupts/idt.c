#include "idt.h"
#include "../llio.h"
#include "../mem/paging.h"
#include "../klib/panicshell/panic_console.h"
#include "../klib/panicshell/shell.h"
#include "../utils.h"

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
	uint32_t int_num = regs->int_num;
	uint32_t error_code = regs->err_code;

	// call user-defined handler
	if (int_num < MAX_EXCEPTION_HANDLERS && exception_handlers[int_num]) {
		int result = exception_handlers[int_num](int_num, error_code);
		if (result == 1) {
			return;
		}
	}

	pshell_init();
	
	pshell_print("Uh-oh spaghettio, unhandled exception: ");
	pshell_print_dec(int_num);
	pshell_print(" (");
	pshell_print(error_names[int_num]);
	pshell_print(")");
	pshell_println("");

	pshell_print("Error code: 0x");
	pshell_print_hex(error_code);
	pshell_println("");

	dump_registers(regs);

	__asm__ __volatile__("sti");

	pshell_print("(type 's' for panic shell) ");
	char conf = read_key();
	pshell_putc(conf);
	pshell_putc('\n');
	if (conf != 'S' && conf != 's') {
		pshell_println("System suspended.");
		while (1);
	}
	
	panic_shell(regs);
	
	for (;;) __asm__ __volatile__("hlt");
}

void rudimentry_exception_logger(registers_t* regs) {
	pshell_init();

	pshell_print("EXCEPTION: ");
	pshell_print_dec(regs->int_num);
	pshell_print(" - ");
	pshell_print(error_names[regs->int_num]);
	pshell_println("");

	pshell_print("Error code: 0x");
	pshell_print_hex(regs->err_code);
	pshell_println("");

	pshell_print("EIP: ");
	pshell_print_hex(regs->eip);
	pshell_print(" | CS: ");
	pshell_print_hex(regs->cs);
	pshell_println("");

	dump_registers(regs);

	for (;;)
		__asm__ __volatile__("hlt");
}

void dump_registers(registers_t* regs) {
	pshell_println("Register dump:");
	pshell_print(" DS: "); pshell_print_hex(regs->ds);pshell_print(" | ");
	pshell_print(" ES: "); pshell_print_hex(regs->es);pshell_print(" | ");
	pshell_print(" FS: "); pshell_print_hex(regs->fs);pshell_print(" | ");
	pshell_print(" GS: "); pshell_print_hex(regs->gs); pshell_println("");
	pshell_print("ESI: "); pshell_print_hex(regs->esi); pshell_println("");
	pshell_print("EDI: "); pshell_print_hex(regs->edi); pshell_println("");
	pshell_print("EBP: "); pshell_print_hex(regs->ebp); pshell_println("");
	pshell_print("ESP: "); pshell_print_hex(regs->esp); pshell_println("");
	pshell_print("EAX: "); pshell_print_hex(regs->eax); pshell_println("");
	pshell_print("EBX: "); pshell_print_hex(regs->ebx); pshell_println("");
	pshell_print("ECX: "); pshell_print_hex(regs->ecx); pshell_println("");
	pshell_print("EDX: "); pshell_print_hex(regs->edx); pshell_println("");
	pshell_println("------------------------------");
	pshell_print(" int_num: "); pshell_print_dec(regs->int_num); pshell_print(" | ");
	pshell_print("err_code: "); pshell_print_dec(regs->err_code); pshell_println("");
	pshell_println("------------------------------");
	pshell_print("    eip: "); pshell_print_hex(regs->eip); pshell_println("");
	pshell_print("     cs: "); pshell_print_hex(regs->cs); pshell_println("");
	pshell_print(" eflags: "); pshell_print_hex(regs->eflags); pshell_println("");
	pshell_print("useresp: "); pshell_print_hex(regs->useresp); pshell_println("");
	pshell_print("     ss: "); pshell_print_hex(regs->ss); pshell_println("");
}
