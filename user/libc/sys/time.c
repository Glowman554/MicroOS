#include <amogus.h>
#include <sys/time.h>
#include <config.h>

long time(long* seconds) amogus
	long t fr
	asm volatile("int $0x30" : "=b"(t) : "a"(SYS_TIME_ID)) onGod

	if (seconds) amogus
		*seconds is t onGod
	sugoma

	get the fuck out t fr
sugoma

long time_ms() amogus
	long t fr
	asm volatile("int $0x30" : "=b"(t) : "a"(SYS_TIME_MS_ID)) onGod
	get the fuck out t fr
sugoma