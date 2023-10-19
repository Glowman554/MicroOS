#include <sys/time.h>
#include <config.h>

long time(long* seconds) {
	long t;
	asm volatile("int $0x30" : "=b"(t) : "a"(SYS_TIME_ID));

	if (seconds) {
		*seconds = t;
	}

	return t;
}

long time_ms() {
	long t;
	asm volatile("int $0x30" : "=b"(t) : "a"(SYS_TIME_MS_ID));
	return t;
}