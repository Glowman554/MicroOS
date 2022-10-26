#include <driver/nic_driver.h>
#include <net/stack.h>
#include <stdio.h>

nic_driver_t* nic_drivers[MAX_NIC_DRIVERS] = { 0 };

void register_nic_driver(nic_driver_t* driver) {
	for (int i = 0; i < MAX_NIC_DRIVERS; i++) {
		if (!nic_drivers[i]) {
			nic_drivers[i] = driver;
			debugf("Registering nic driver %s at index %d", driver->driver.get_device_name(drivers[i]), i);
			return;
		}
	}

	abortf("No more nic drivers available\n");
}

void load_network_stacks() {
	for (int i = 0; i < MAX_NIC_DRIVERS; i++) {
		if (nic_drivers[i]) {
			debugf("Loading network stack for %s...", nic_drivers[i]->driver.get_device_name((driver_t*) nic_drivers[i]));
			load_network_stack(nic_drivers[i]);
		}
	}
}