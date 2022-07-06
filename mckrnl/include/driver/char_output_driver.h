#pragma once

#include <driver/driver.h>

typedef struct char_output_driver {
	driver_t driver;
	void (*putc)(struct char_output_driver*, char);
} char_output_driver_t;

extern char_output_driver_t* global_char_output_driver;