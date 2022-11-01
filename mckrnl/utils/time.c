#include <utils/time.h>
#include <stdio.h>

void time_format(char* out, time_t* time) {
    sprintf(out, "%d:%d:%d %d/%d/%d", time->hours, time->minutes, time->seconds, time->day, time->month, time->year);
}
