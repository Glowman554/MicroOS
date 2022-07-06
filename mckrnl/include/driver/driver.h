#pragma once

#include <stdbool.h>
#include <config.h>

typedef struct driver {
	bool (*is_device_present)(struct driver*);
	char* (*get_device_name)(struct driver*);
	void (*init)(struct driver*);

	void* driver_specific_data;
} driver_t;

extern driver_t* drivers[MAX_DRIVERS];

void register_driver(driver_t* driver);
void activate_drivers();