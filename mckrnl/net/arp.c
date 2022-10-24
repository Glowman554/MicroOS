#include "driver/nic_driver.h"
#include <net/stack.h>
#include <net/arp.h>

#include <stdio.h>

void arp_etherframe_recv(ether_frame_handler_t* handler, uint8_t* payload, uint32_t size) {
	if (size < sizeof(arp_message_t)) {
		return;
	}

	network_stack_t* stack = handler->data;

	arp_message_t* arp = (arp_message_t*) payload;
	if (arp->hardware_type == 0x0100) {
		if(arp->protocol == 0x0008 && arp->hardware_address_size == 6 && arp->protocol_address_size == 4 && arp->dest_ip == stack->driver->ip.ip) {
			switch (arp->command) {
				case 0x0100: // request
					{
						arp->command = 0x0200;
						arp->dest_ip = arp->src_ip;
						arp->dest_mac = arp->src_mac;
						arp->src_ip = stack->driver->ip.ip;
						arp->src_mac = stack->driver->mac.mac;
						ehterframe_send(&stack->arp.handler, stack->driver, arp->dest_mac, (uint8_t*) arp,  sizeof(arp_message_t));
					}
					break;
				case 0x0200: // response
					{
						if (stack->arp.num_cache_entry < 128) {
							stack->arp.ip_cache[stack->arp.num_cache_entry] = (ip_u) { .ip = arp->src_ip };
							stack->arp.mac_cache[stack->arp.num_cache_entry] = (mac_u) { .mac = arp->src_mac };
							stack->arp.num_cache_entry++;
						}
					}
					break;
			}
		}
	}
}

void arp_broadcast_mac(arp_provider_t* provider, nic_driver_t* driver, ip_u ip) {
	arp_message_t arp = {
		.hardware_type = 0x0100,
		.protocol = 0x0008,
		.hardware_address_size = 6,
		.protocol_address_size = 4,
		.command = 0x0200,
		.src_mac = driver->mac.mac,
		.src_ip = driver->ip.ip,
		.dest_mac = arp_resolve(provider, driver, ip).mac,
		.dest_ip = ip.ip
	};

	ehterframe_send(&provider->handler, driver, arp.dest_mac, (uint8_t*) &arp,  sizeof(arp_message_t));
}

void arp_request_mac(arp_provider_t* provider, nic_driver_t* driver, ip_u ip) {
		arp_message_t arp = {
		.hardware_type = 0x0100,
		.protocol = 0x0008,
		.hardware_address_size = 6,
		.protocol_address_size = 4,
		.command = 0x0100,
		.src_mac = driver->mac.mac,
		.src_ip = driver->ip.ip,
		.dest_mac = 0xFFFFFFFFFFFF,
		.dest_ip = ip.ip
	};

	ehterframe_send(&provider->handler, driver, arp.dest_mac, (uint8_t*) &arp,  sizeof(arp_message_t));
}

mac_u arp_get_mac_from_cache(arp_provider_t* provider, ip_u ip) {
		for (int i = 0; i < provider->num_cache_entry; i++) {
		if (provider->ip_cache[i].ip == ip.ip) {
			return provider->mac_cache[i];
		}
	} 
	mac_u m = { .mac = 0xFFFFFFFFFFFF };
	return m;
}

mac_u arp_resolve(arp_provider_t* provider, nic_driver_t* driver, ip_u ip) {
	if (ip.ip == 0xffffffff) {
		mac_u m = { .mac = 0xFFFFFFFFFFFF };
		return m;
	}

	mac_u result = arp_get_mac_from_cache(provider, ip);

	if (result.mac == 0xFFFFFFFFFFFF) {
		arp_request_mac(provider, driver, ip);
	}

	int timeout = 10000000;
	note("this isnt the best way to implement a timeout. FIXME!");
	
	while (result.mac == 0xFFFFFFFFFFFF) {
		result = arp_get_mac_from_cache(provider, ip);
		if (--timeout == 0) {
			debugf("timeout for arp request!\n");
			mac_u m = { .mac = 0 };
			return m;
		}
	}

	return result;
}