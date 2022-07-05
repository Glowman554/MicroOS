#define uint16_t unsigned short


void puts(char* str) {
	int i = 0;
	while (str[i] != '\0') {
		asm("int $0x30" : : "a" (1 /* SYS_PUTC */), "b" (str[i]));
		i++;
	}
}

void _start(void) {
	puts("Hello, world from a userspace program!\n");

	puts("Trying to write to vram now! SHOULD CAUSE PAGEFAULT!\n");

	uint16_t* videomem = (uint16_t*) 0xb8000;
	for (int i = 0; i < 5; i++) {
		*videomem++ = (0x07 << 8) | ('0' + i);
	}

    while(1);
}