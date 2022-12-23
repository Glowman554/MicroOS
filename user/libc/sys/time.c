#include <sys/time.h>
#include <config.h>

long time() {
	long t;
	asm volatile("int $0x30" : "=b"(t) : "a"(SYS_TIME_ID));
	return t;
}
