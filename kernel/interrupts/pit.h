#include <stdint.h>

#define PIT_CHANNEL0      0x40
#define PIT_COMMAND_PORT  0x43

#define PIT_IRQ_NUM       0 // maps to IRQ0 (interrupt vector 32)

#define PIT_FREQUENCY     1193182 // Hz

void pit_init(uint32_t hz);