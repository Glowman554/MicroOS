#pragma once

typedef struct clock_result {
	int seconds;
	int minutes;
	int hours;
	int day;
	int month;
	int year;
} clock_result_t;

typedef struct clock_result time_t;

int time_format(char* output, time_t* time);
