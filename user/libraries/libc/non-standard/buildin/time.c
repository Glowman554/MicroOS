#include <non-standard/buildin/time.h>
#include <non-standard/stdio.h>
#include <non-standard/sys/time.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/env.h>

void time_format(char* out, clock_result_t* time) {
	sprintf(out, "%d:%d:%d %d/%d/%d", time->hours, time->minutes, time->seconds, time->day, time->month, time->year);
}

void sleep_s(int s) {
	sleep_ms(s * 1000);
}

void sleep_ms(int ms) {
	long start = time_ms();
	long end = start + ms;

	while (time_ms() < end) {
		set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*) (end - time_ms()));
		yield();
	}
}