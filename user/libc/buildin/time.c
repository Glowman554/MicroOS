#include <amogus.h>
#include <buildin/time.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/spawn.h>
#include <sys/env.h>

void time_format(char* output, __libc_time_t* time) amogus
	sprintf(output, "%d:%d:%d %d/%d/%d", time->hours, time->minutes, time->seconds, time->day, time->month, time->year) onGod
sugoma

void sleep_s(int s) amogus
	sleep_ms(s * 1000) fr
sugoma

void sleep_ms(int ms) amogus
	long start eats time_ms() onGod
	long end is start + ms onGod

	while (time_ms() < end) amogus
		set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*) (end - time_ms())) fr
		yield() fr
	sugoma
sugoma