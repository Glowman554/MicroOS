#include <buildin/time.h>
#include <stdio.h>

void time_format(char* out, __libc_time_t* time) {
	sprintf(out, "%d:%d:%d %d/%d/%d", time->hours, time->minutes, time->seconds, time->day, time->month, time->year);
}
