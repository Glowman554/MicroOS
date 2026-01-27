#pragma once

#include <stdint.h>
#include <driver/power_driver.h>

typedef struct simple_power_driver {
    power_driver_t driver;
    uint16_t SLP_TYPa;
    uint16_t SLP_TYPb;
} simple_power_driver_t;

extern simple_power_driver_t simple_power_driver;