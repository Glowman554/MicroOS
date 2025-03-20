#include <driver/nic_driver.h>
#include <net/stack.h>
#include <stdio.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <config.h>
#include <devices/nic.h>

nic_driver_t** nic_drivers = NULL;
int num_nic_drivers = 0;

void register_nic_driver(nic_driver_t* driver) {
	nic_drivers = vmm_resize(sizeof(nic_driver_t*), num_nic_drivers, num_nic_drivers + 1, nic_drivers);
	nic_drivers[num_nic_drivers] = driver;
	num_nic_drivers++;
}

void load_network_stacks() {
	for (int i = 0; i < num_nic_drivers; i++) {
	#ifdef NETWORK_STACK
		debugf("Loading network stack for %s...", nic_drivers[i]->driver.get_device_name((driver_t*) nic_drivers[i]));
		load_network_stack(nic_drivers[i]);
		devfs_register_file(&global_devfs, create_nic_file(nic_drivers[i], i));
	#endif
	}
}