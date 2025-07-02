#include <stdint.h>

typedef struct {
	uint32_t esp;
	void (*ret_addr)(void);
} KernelContext;

extern KernelContext kernel_ctx;

extern void switch_to_user_mode(uint32_t e, uint32_t s);