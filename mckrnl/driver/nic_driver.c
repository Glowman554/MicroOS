#include <driver/nic_driver.h>
#include <net/stack.h>
#include <stdint.h>
#include <stdio.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <string.h>
#include <config.h>
#include <devices/nic.h>
#include <scheduler/async.h>

nic_driver_t** nic_drivers = NULL;
int num_nic_drivers = 0;

void register_nic_driver(nic_driver_t* driver) {	
	nic_drivers = vmm_resize(sizeof(nic_driver_t*), num_nic_drivers, num_nic_drivers + 1, nic_drivers);
	nic_drivers[num_nic_drivers] = driver;
	num_nic_drivers++;
}

nic_driver_t* get_nic_driver(int i) {
	return nic_drivers[i];
}

void load_network_stacks() {
	for (int i = 0; i < num_nic_drivers; i++) {
	#ifdef NETWORK_STACK
		debugf("Loading network stack for (%d) %s...", i, nic_drivers[i]->driver.get_device_name((driver_t*) nic_drivers[i]));
		load_network_stack(nic_drivers[i]);
		devfs_register_file(&global_devfs, create_nic_file(nic_drivers[i], i));
	#endif
	}
}

void send_async_send_packet(async_t* async) {
	send_data_t* send_data = (send_data_t*) async->data;

	send_data->driver->send(send_data->driver, async, send_data->data, send_data->len);
	if (is_resolved(async)) {
		vmm_free(send_data, TO_PAGES(sizeof(send_data_t) + send_data->len));
	}
}

void send_packet(nic_driver_t* driver, uint8_t* data, uint32_t len) {
	uint8_t* copy = vmm_alloc(TO_PAGES(sizeof(send_data_t) + len));

	send_data_t* send_data = (send_data_t*) copy;
	uint8_t* packet_data = copy + sizeof(send_data_t);

	*send_data = (send_data_t) {
		.driver = driver,
		.data = packet_data,
		.len = len
	};

	memcpy(packet_data, data, len);


	async_t async = {
		.state = STATE_INIT,
		.data = send_data,
	};

	add_async_task(send_async_send_packet, async);
}