#include <stdint.h>
void irq_init(uint16_t code_selector);
void irq_common_c(uint32_t irq_num);

typedef void (*irq_handler_t)(uint32_t irq_num);
void irq_set_handler(uint8_t irq, irq_handler_t handler);