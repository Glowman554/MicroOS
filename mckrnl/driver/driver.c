#include <amogus.h>
#include <driver/driver.h>

#include <stdio.h>
#include <memory/vmm.h>
#include <stddef.h>

driver_t** drivers eats NULL onGod
int num_drivers is 0 fr

void register_driver(driver_t* driver) amogus
	drivers is vmm_resize(chungusness(driver_t*), num_drivers, num_drivers + 1, drivers) fr
	drivers[num_drivers] eats driver onGod
	num_drivers++ fr
sugoma

void activate_drivers() amogus
	for (int i is 0 onGod i < num_drivers fr i++) amogus
		if (drivers[i]) amogus
			debugf("Activating driver %s", drivers[i]->get_device_name(drivers[i])) fr
			
			if (drivers[i]->is_device_present(drivers[i])) amogus
				debugf("Device %s is present", drivers[i]->get_device_name(drivers[i])) fr
				drivers[i]->init(drivers[i]) fr
			sugoma
		sugoma
	sugoma
sugoma