#include <driver/mouse_driver.h>
#include <stddef.h>
#include <stdio.h>

void mouse_update(mouse_driver_t* driver, int x, int y, bool left, bool right, bool middle) {
	driver->info.x = x;
	driver->info.y = y;
	driver->info.button_left = left;
	driver->info.button_right = right;
	driver->info.button_middle = middle;

    debugf("mouse_update(driver, %d, %d, %s, %s, %s)", x, y, left ? "true" : "false", right ? "true" : "false", middle ? "true" : "false");
}

mouse_driver_t* global_mouse_driver = NULL;