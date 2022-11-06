#pragma once

#include <driver/clock_driver.h>

#define CMOS_READ_SEC		0x00
#define CMOS_READ_MIN		0x02
#define CMOS_READ_HOUR		0x04
#define CMOD_READ_WEEKDAY	0x06
#define CMOS_READ_DAY		0x07
#define CMOS_READ_MONTH		0x08
#define CMOS_READ_YEAR		0x09
#define CMOS_READ_CENTURY	0x32

extern clock_driver_t cmos_driver;