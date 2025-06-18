#include "uptime.h"
#include "../../interrupts/idt.h"
#include "../../interrupts/pic.h"

static volatile uint32_t ticks = 0; // 4294967295 ticks total ~497 days
static int tick_rate = 100;  // default is 100 Hz

void pit_uptime_handler(uint32_t irq_num) {
	(void)irq_num;
	++ticks;
}

uint32_t uptime_ticks(void) {
	return ticks;
}

uint32_t uptime_seconds(void) {
	return ticks / tick_rate;
}
