#include <sys/file.h>

#include <config.h>

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

int filesize(int fd) {
	int size = -1;
	asm volatile("int $0x30" : "=c"(size) : "a"(SYS_FILESIZE_ID), "b"(fd));
	return size;
}