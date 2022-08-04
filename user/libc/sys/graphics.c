#include <sys/graphics.h>

#include <config.h>

int vmode() {
	int mode;
	asm volatile("int $0x30" : "=b"(mode) : "a"(SYS_VMODE_ID));
	return mode;
}

void vpoke(uint32_t offset, uint8_t val) {
	asm volatile("int $0x30" : : "a"(SYS_VPOKE_ID), "b"(offset), "c"(val));
}