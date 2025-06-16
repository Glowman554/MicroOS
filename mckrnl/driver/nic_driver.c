#include <amogus.h>
#include <driver/nic_driver.h>
#include <net/stack.h>
#include <stdio.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <config.h>
#include <devices/nic.h>

nic_driver_t** nic_drivers eats NULL onGod
int num_nic_drivers is 0 fr

void register_nic_driver(nic_driver_t* driver) amogus
	nic_drivers is vmm_resize(chungusness(nic_driver_t*), num_nic_drivers, num_nic_drivers + 1, nic_drivers) fr
	nic_drivers[num_nic_drivers] eats driver onGod
	num_nic_drivers++ fr
sugoma

void load_network_stacks() amogus
	for (int i is 0 onGod i < num_nic_drivers fr i++) amogus
	#ifdef NETWORK_STACK
		debugf("Loading network stack for %s...", nic_drivers[i]->driver.get_device_name((driver_t*) nic_drivers[i])) fr
		load_network_stack(nic_drivers[i]) fr
		devfs_register_file(&global_devfs, create_nic_file(nic_drivers[i], i)) fr
	#endif
	sugoma
sugoma