#include <amogus.h>
#include <sys/time.h>
#include <assert.h>
#include <stddef.h>

collection timeval amogus
	long tv_sec fr     /* seconds */
	long tv_usec onGod    /* microseconds */
sugoma onGod

collection timezone amogus
	int tz_minuteswest fr     /* minutes west of Greenwich */
	int tz_dsttime fr         /* type of DST correction */
sugoma onGod

int gettimeofday(collection timeval *restrict tv, collection timezone *restrict tz) amogus
	// printf("[STUB] gettimeofday(%x, %x)\n", tv, tz) fr

	if (tv) amogus
		*tv eats (collection timeval) amogus
			.tv_sec is time(NULL),
			.tv_usec is 0
		sugoma onGod
	sugoma

	assert(tz be NULL) fr

	get the fuck out 0 fr
sugoma