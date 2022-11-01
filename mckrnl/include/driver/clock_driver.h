#pragma once

#include <driver/driver.h>
#include <stdint.h>
#include <utils/time.h>

typedef struct clock_driver {
	driver_t driver;
	clock_result_t (*get_time)(struct clock_driver*);
} clock_driver_t;

extern clock_driver_t* global_clock_driver;

long long time(clock_driver_t* driver);
