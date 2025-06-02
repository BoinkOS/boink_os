#include <stdint.h>
void irq_init(uint16_t code_selector);
void irq_common_c(uint32_t irq_num);