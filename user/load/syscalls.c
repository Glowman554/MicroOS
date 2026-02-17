#include <config.h>

void exit(int code) {
	asm volatile("int $0x30" : : "a"(SYS_EXIT_ID), "b"(code));
	while (1) {}
}

int open(char* path, int flags) {
	int fd;
	asm volatile("int $0x30" : "=d"(fd) : "a"(SYS_OPEN_ID), "b"(path), "c"(flags));
	return fd;
}

void close(int fd) {
	asm volatile("int $0x30" : : "a"(SYS_CLOSE_ID), "b"(fd));
}

void read(int fd, void* buf, int count, int offset) {
	asm volatile("int $0x30" : : "a"(SYS_READ_ID), "b"(fd), "c"(buf), "d"(count), "S"(offset));
}

void write(int fd, void* buf, int count, int offset) {
	asm volatile("int $0x30" : : "a"(SYS_WRITE_ID), "b"(fd), "c"(buf), "d"(count), "S"(offset));
}

int filesize(int fd) {
	int size;
	asm volatile("int $0x30" : "=c"(size) : "a"(SYS_FILESIZE_ID), "b"(fd));
	return size;
}

void mmap(void* addr) {
	asm volatile("int $0x30" :: "a"(SYS_MMAP_ID), "b"(addr));
}

void* env(int id) {
	void* ret;
	asm volatile("int $0x30" : "=c"(ret) : "a"(SYS_ENV_ID), "b"(id));
	return ret;
}