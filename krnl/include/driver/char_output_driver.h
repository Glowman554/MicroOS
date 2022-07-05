#pragma once

#include <driver/driver.h>

typedef struct char_output_driver {
	driver_t driver;
	void (*putc)(struct char_output_driver*, char);
} char_output_driver_t;