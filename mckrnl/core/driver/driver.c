#include <driver/driver.h>

#include <stdio.h>
#include <memory/heap.h>
#include <stddef.h>

driver_t** drivers = NULL;
int num_drivers = 0;

void register_driver(driver_t* driver) {
	drivers = krealloc(drivers, sizeof(driver_t*) * (num_drivers + 1));
	drivers[num_drivers] = driver;
	num_drivers++;
}

void activate_drivers() {
	for (int i = 0; i < num_drivers; i++) {
		if (drivers[i]) {
			debugf("Activating driver %s", drivers[i]->get_device_name(drivers[i]));
			
			if (drivers[i]->is_device_present(drivers[i])) {
				debugf("Device %s is present", drivers[i]->get_device_name(drivers[i]));
				drivers[i]->init(drivers[i]);
			}
		}
	}
}