void kmain(void) {
    char* video_mem = (char*) 0xB8000;
    const char* msg = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

    for (int i = 0; msg[i] != '\0'; i++) {
        video_mem[i * 2] = msg[i];      // character
        video_mem[i * 2 + 1] = 0x2F;    // white on green
    }

    while (1);  // freeze so the CPU doesn’t start interpreting RAM
}