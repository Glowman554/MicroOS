#include <sys/getc.h>
#include <config.h>

char async_getc() {
	char c;
	asm volatile("int $0x30" : "=b"(c) : "a"(SYS_ASYNC_GETC_ID));
	return c;
}

char async_getarrw() {
	char c;
	asm volatile("int $0x30" : "=b"(c) : "a"(SYS_ASYNC_GETARRW_ID));
	return c;
}