#pragma once

#include <stdint.h>
#include <driver/driver.h>
#include <config.h>
#include <async.h>

typedef union ip {
	uint8_t ip_p[4];
	uint32_t ip;
} ip_u;

typedef union mac {
	uint8_t mac_p[6];
	uint64_t mac;
} mac_u;

typedef struct ip_configuration {
	ip_u ip;
	ip_u subnet_mask;
	ip_u gateway_ip;
	ip_u dns_ip;
} ip_configuration_t;

#define NOMAC 0xFFFFFFFFFFFF
#define NOIP 0xFFFFFFFF


typedef struct nic_driver {
	driver_t driver; // the driver_specific_data is reserved for a poniter to a network_stack_t ptr!

	void (*send)(struct nic_driver* driver, async_t* async, uint8_t* data, uint32_t len);
	void (*recv)(struct nic_driver* driver, uint8_t* data, uint32_t len); // this is a callback function CALLED BY THE DRIVER!

	void (*stack)(struct nic_driver* driver, void* stack);

	ip_configuration_t ip_config;
	mac_u mac;
} nic_driver_t;

typedef struct send_data {
	nic_driver_t* driver;
	uint8_t* data;
	uint32_t len;

	int driver_specific;
} send_data_t;

extern nic_driver_t** nic_drivers;
extern int num_nic_drivers;

void register_nic_driver(nic_driver_t* driver);
nic_driver_t* get_nic_driver(int i);

void send_packet(nic_driver_t* driver, uint8_t* data, uint32_t len);

void load_network_stacks();