#include <sys/time.h>
#include <assert.h>
#include <stddef.h>

struct timeval {
	long tv_sec;     /* seconds */
	long tv_usec;    /* microseconds */
};

struct timezone {
	int tz_minuteswest;     /* minutes west of Greenwich */
	int tz_dsttime;         /* type of DST correction */
};

int gettimeofday(struct timeval *restrict tv, struct timezone *restrict tz) {
	// printf("[STUB] gettimeofday(%x, %x)\n", tv, tz);

	if (tv) {
		*tv = (struct timeval) {
			.tv_sec = time(NULL),
			.tv_usec = 0
		};
	}

	assert(tz == NULL);

	return 0;
}