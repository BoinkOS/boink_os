#include <stdint.h>

int check_glfs_magic(uint8_t* buffer) {
	const char* expected = "GLFSv0\n";
	int valid = 1;

	for (int i = 0; i < 7; i++) {
		if (buffer[i] != expected[i]) {
			valid = 0;
			break;
		}
	}

	if (valid) {
		return 1;
	} else {
		return 0;
	}
}