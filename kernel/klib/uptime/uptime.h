#include <stdint.h>

void uptime_install(uint32_t hz, uint16_t selector);

uint32_t uptime_ticks(void);
uint32_t uptime_seconds(void);