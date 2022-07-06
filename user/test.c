#define uint16_t unsigned short

#define SYS_OPEN_ID 0x1
#define SYS_CLOSE_ID 0x2
#define SYS_READ_ID 0x3
#define SYS_WRITE_ID 0x4
#define SYS_ASYNC_GETC_ID 0x5

int open(char* path, int flags) {
	int fd = -1;
	asm volatile("int $0x30" : "=d"(fd) : "a"(SYS_OPEN_ID), "b"(path), "c"(flags));
	return fd;
}

void close(int fd) {
	asm volatile("int $0x30" : : "a"(SYS_CLOSE_ID), "b"(fd));
}

void read(int fd, void* buf, int count, int offset) {
	if (fd == 0) { // stdin is a special case on this kernel
		char* buffer = (char*) buf;
		for (int i = 0; i < count; i++) {
			while (buffer[i] == 0) {
				char c;
				asm volatile("int $0x30" : "=b"(c) : "a"(SYS_ASYNC_GETC_ID));
				buffer[i] = c;
			}
		}
	} else {
	asm volatile("int $0x30" : : "a"(SYS_READ_ID), "b"(fd), "c"(buf), "d"(count), "S"(offset));
	}
}

void write(int fd, void* buf, int count, int offset) {
	asm volatile("int $0x30" : : "a"(SYS_WRITE_ID), "b"(fd), "c"(buf), "d"(count), "S"(offset));
}

int strlen(char* str) {
	int len = 0;
	while (str[len]) {
		len++;
	}
	return len;
}

void puts(char* str) {
	write(1, str, strlen(str), 0);
}

void _start(void) {
	puts("Hello, world from a userspace program!\n");

	// puts("Trying to write to vram now! SHOULD CAUSE PAGEFAULT!\n");

	// uint16_t* videomem = (uint16_t*) 0xb8000;
	// for (int i = 0; i < 5; i++) {
	// 	*videomem++ = (0x07 << 8) | ('0' + i);
	// }

	while (1) {
		char buf[2] = {0};
		read(0, buf, 1, 0);
		write(1, buf, 1, 0);
	}

    while(1);
}