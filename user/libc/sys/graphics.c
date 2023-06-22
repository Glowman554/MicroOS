#include <sys/graphics.h>

#include <config.h>

int vmode() {
	int mode;
	asm volatile("int $0x30" : "=b"(mode) : "a"(SYS_VMODE_ID));
	return mode;
}

void vpoke(uint32_t offset, uint8_t* val, uint32_t range) {
	asm volatile("int $0x30" : : "a"(SYS_VPOKE_ID), "b"(offset), "c"(val), "d"(range));
}

void vcursor(int x, int y) {
	asm volatile("int $0x30" : : "a"(SYS_VCURSOR_ID), "b"(x), "c"(y));
}

void set_color(char* color, bool background) {
	asm volatile("int $0x30" : : "a"(SYS_SET_COLOR_ID), "b"(color), "c"(background));
}