#pragma once

#include <driver/nic_driver.h>
#include <net/etherframe.h>

typedef struct network_stack {
	ether_frame_provider_t ehter_frame;
} network_stack_t;

void load_network_stack(nic_driver_t* nic);
