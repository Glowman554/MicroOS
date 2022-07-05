#pragma once

#include <driver/driver.h>

typedef struct char_input_driver {
	driver_t driver;
	char (*getc)(struct char_input_driver*);
} char_input_driver_t;