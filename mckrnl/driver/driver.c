#include <driver/driver.h>

#include <stdio.h>

driver_t* drivers[MAX_DRIVERS] = { 0 };

void register_driver(driver_t* driver) {
	for (int i = 0; i < MAX_DRIVERS; i++) {
		if (!drivers[i]) {
			drivers[i] = driver;
			debugf("Registering driver %s at index %d", driver->get_device_name(drivers[i]), i);
			return;
		}
	}

	abortf("No more drivers available\n");
}

void activate_drivers() {
	for (int i = 0; i < MAX_DRIVERS; i++) {
		if (drivers[i]) {
			debugf("Activating driver %s", drivers[i]->get_device_name(drivers[i]));
			
			if (drivers[i]->is_device_present(drivers[i])) {
				debugf("Device %s is present", drivers[i]->get_device_name(drivers[i]));
				drivers[i]->init(drivers[i]);
			}
		}
	}
}