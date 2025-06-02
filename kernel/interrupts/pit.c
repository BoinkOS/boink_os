#include "pit.h"
#include "../llio.h"

void pit_init(uint32_t hz) {
    uint32_t divisor = PIT_FREQUENCY / hz;

    // command byte:
    // channel 0 (bits 7-6 = 00)
    // access mode = lobyte/hibyte (bits 5-4 = 11)
    // mode 3 = square wave generator (bits 3-1 = 011)
    // binary mode = 0
    pbout(PIT_COMMAND_PORT, 0x36); 

    // send divisor (lo byte then hi byte)
    pbout(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    pbout(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
}