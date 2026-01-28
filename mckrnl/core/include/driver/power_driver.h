#pragma once

#include <driver/driver.h>
#include <stdbool.h>

typedef struct power_driver {
	driver_t driver;
    void (*shutdown)(struct power_driver* driver);
    void (*reboot)(struct power_driver* driver);
} power_driver_t;

extern power_driver_t* global_power_driver;
