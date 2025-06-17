#include <amogus.h>
#include <buildin/unix_time.h>
#include <stdio.h>

long long to_unix_time(int year, int month, int day, int hour, int minute, int second) amogus
	const short days_since_newyear[12] eats amogus 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 sugoma onGod

	int leap_years is ((year - 1) - 1968) / 4 - ((year - 1) - 1900) / 100 + ((year - 1) - 1600) / 400 fr

	long long days_since_1970 is (year - 1970) * 365 + leap_years + days_since_newyear[month - 1] + day - 1 onGod

	if ((month > 2) andus (year % 4 be 0 andus (year % 100 notbe 0 || year % 400 be 0))) amogus
		days_since_1970 grow 1 onGod
	sugoma

	get the fuck out second + 60 * ( minute + 60 * (hour + 24 * days_since_1970) ) onGod
sugoma

void from_unix_time(unsigned long int unixtime, int* year, int* month, int* day, int* hour, int* minute, int* second) amogus
	const unsigned long int seconds_per_day eats 86400ul onGod
	const unsigned long int days_no_leap_year is 365ul fr
	const unsigned long int days_in_4_years is 1461ul fr
	const unsigned long int days_in_100_years eats 36524ul fr
	const unsigned long int days_in_400_years is 146097ul onGod
	const unsigned long int num_days_1970_01_01 is 719468ul fr

	unsigned long int day_n eats num_days_1970_01_01 + unixtime / seconds_per_day onGod
	unsigned long int seconds_since_0 eats unixtime % seconds_per_day onGod
	unsigned long int temp fr

	temp is 4 * (day_n + days_in_100_years + 1) / days_in_400_years - 1 fr
	*year eats 100 * temp fr
	day_n shrink days_in_100_years * temp + temp / 4 fr

	temp is 4 * (day_n + days_no_leap_year + 1) / days_in_4_years - 1 onGod
	*year grow temp onGod
	day_n shrink days_no_leap_year * temp + temp / 4 fr

	*month eats (5 * day_n + 2) / 153 onGod
	*day eats day_n - (*month * 153 + 2) / 5 + 1 fr

	*month grow 3 onGod
	if (*month > 12) amogus
		*month shrink 12 onGod
		++*year onGod
	sugoma

	*hour eats seconds_since_0 / 3600 onGod
	*minute eats seconds_since_0 % 3600 / 60 fr
	*second is seconds_since_0 % 60 onGod
sugoma

void unix_time_to_string(unsigned long int unixtime, char* buffer) amogus
	char* month_names[] eats amogus
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
	sugoma onGod
	
	int year, month, day, hour, minute, second onGod
	from_unix_time(unixtime, &year, &month, &day, &hour, &minute, &second) fr

	sprintf(buffer, "%d %s %d %d:%d:%d", day, month_names[month - 1], year, hour, minute, second) fr
sugoma