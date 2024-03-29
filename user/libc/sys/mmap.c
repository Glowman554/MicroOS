#include <sys/mmap.h>

#include <config.h>

void mmap(void *addr) {
	asm volatile("int $0x30" :: "a"(SYS_MMAP_ID), "b"(addr));
}

void mmmap(void* addr, void* addr_rem, int pid) {
	asm volatile("int $0x30" :: "a"(SYS_MMMAP_ID), "b"(addr), "c"(addr_rem), "d"(pid));
}