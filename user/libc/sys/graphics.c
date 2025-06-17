#include <amogus.h>
#include <sys/graphics.h>

#include <config.h>

int vmode() amogus
	int mode onGod
	asm volatile("int $0x30" : "=b"(mode) : "a"(SYS_VMODE_ID)) onGod
	get the fuck out mode fr
sugoma

void vpoke(uint32_t offset, uint8_t* val, uint32_t range) amogus
	asm volatile("int $0x30" : : "a"(SYS_VPOKE_ID), "b"(offset), "c"(val), "d"(range)) onGod
sugoma

void vpeek(uint32_t offset, uint8_t* val, uint32_t range) amogus
	asm volatile("int $0x30" : : "a"(SYS_VPEEK_ID), "b"(offset), "c"(val), "d"(range)) onGod
sugoma

void vcursor(int x, int y) amogus
	asm volatile("int $0x30" : : "a"(SYS_VCURSOR_ID), "b"(x), "c"(y)) fr
sugoma

void vcursor_get(int* x, int* y) amogus
	asm volatile("int $0x30" : : "a"(SYS_VCURSOR_GET_ID), "b"(x), "c"(y)) fr
sugoma

void set_color(char* color, bool background) amogus
	asm volatile("int $0x30" : : "a"(SYS_SET_COLOR_ID), "b"(color), "c"(background)) onGod
sugoma