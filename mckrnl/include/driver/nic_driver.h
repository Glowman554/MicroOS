#pragma once

#include <stdint.h>
#include <driver/driver.h>
#include <config.h>

typedef union ip {
	uint8_t ip_p[4];
	uint32_t ip;
} ip_u;

typedef union mac {
	uint8_t mac_p[6];
	uint64_t mac;
} mac_u;

typedef struct nic_driver {
	driver_t driver; // the driver_specific_data is reserved for a poniter to a network_stack_t ptr!

	void (*send)(struct nic_driver* driver, uint8_t* data, uint32_t len);
	void (*recv)(struct nic_driver* driver, uint8_t* data, uint32_t len); // this is a callback function CALLED BY THE DRIVER!

	ip_u ip;
	mac_u mac;
} nic_driver_t;

extern nic_driver_t** nic_drivers;
extern int num_nic_drivers;

void register_nic_driver(nic_driver_t* driver);

void load_network_stacks();