#pragma once

typedef struct clock_result {
	int seconds;
	int minutes;
	int hours;
	int day;
	int month;
	int year;
} clock_result_t;

typedef struct clock_result __libc_time_t;

void time_format(char* output, __libc_time_t* time);

void sleep_s(int s);
void sleep_ms(int ms);