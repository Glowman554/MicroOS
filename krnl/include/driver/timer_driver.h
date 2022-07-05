#pragma once

#include <driver/driver.h>
#include <stdint.h>

typedef struct timer_driver {
	driver_t driver;
	void (*sleep)(struct timer_driver*, uint32_t ms);
} timer_driver_t;