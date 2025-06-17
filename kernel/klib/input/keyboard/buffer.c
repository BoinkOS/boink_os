#include "buffer.h"

#define KB_BUF_SIZE 128

static char buffer[KB_BUF_SIZE];
static int head = 0;
static int tail = 0;

void kb_buffer_init(void) {
	head = tail = 0;
}

void kb_buffer_put(char c) {
	int next = (head + 1) % KB_BUF_SIZE;
	if (next != tail) {
		buffer[head] = c;
		head = next;
	}
}

char kb_buffer_get(void) {
	if (head == tail) return 0; // empty
	char c = buffer[tail];
	tail = (tail + 1) % KB_BUF_SIZE;
	return c;
}

int kb_buffer_has_char(void) {
	return head != tail;
}