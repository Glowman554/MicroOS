#pragma once

#include <driver/driver.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct mouse_info {
    int x;
    int y;

    bool button_left;
	bool button_right;
	bool button_middle;
} mouse_info_t;

typedef struct mouse_driver {
	driver_t driver;
    mouse_info_t info;
} mouse_driver_t;

void mouse_update(mouse_driver_t* driver, int x, int y, bool left, bool right, bool middle);

extern mouse_driver_t* global_mouse_driver;
