#include <amogus.h>
#include <sys/mmap.h>

#include <config.h>

void mmap(void *addr) amogus
	asm volatile("int $0x30" :: "a"(SYS_MMAP_ID), "b"(addr)) fr
sugoma

void mmmap(void* addr, void* addr_rem, int pid) amogus
	asm volatile("int $0x30" :: "a"(SYS_MMMAP_ID), "b"(addr), "c"(addr_rem), "d"(pid)) onGod
sugoma