#include <driver/nic_driver.h>
#include <net/stack.h>
#include <net/arp.h>

#include <memory/vmm.h>
#include <string.h>

#include <stdio.h>

void arp_etherframe_recv(ether_frame_handler_t* handler, uint8_t* payload, uint32_t size) {
	if (size < sizeof(arp_message_t)) {
		return;
	}

	arp_message_t* arp = (arp_message_t*) payload;
	if (arp->hardware_type == 0x0100) {
		if(arp->protocol == 0x0008 && arp->hardware_address_size == 6 && arp->protocol_address_size == 4 && arp->dest_ip == handler->stack->driver->ip.ip) {
			switch (arp->command) {
				case 0x0100: // request
					{
						arp->command = 0x0200;
						arp->dest_ip = arp->src_ip;
						arp->dest_mac = arp->src_mac;
						arp->src_ip = handler->stack->driver->ip.ip;
						arp->src_mac = handler->stack->driver->mac.mac;
						etherframe_send(&handler->stack->arp->handler, handler->stack, arp->dest_mac, (uint8_t*) arp,  sizeof(arp_message_t));
					}
					break;
				case 0x0200: // response
					{
						if (handler->stack->arp->num_cache_entry < 128) {
							handler->stack->arp->ip_cache[handler->stack->arp->num_cache_entry] = (ip_u) { .ip = arp->src_ip };
							handler->stack->arp->mac_cache[handler->stack->arp->num_cache_entry] = (mac_u) { .mac = arp->src_mac };
							handler->stack->arp->num_cache_entry++;
						}
					}
					break;
			}
		}
	}
}

void arp_broadcast_mac(network_stack_t* stack, ip_u ip) {
	arp_message_t arp = {
		.hardware_type = 0x0100,
		.protocol = 0x0008,
		.hardware_address_size = 6,
		.protocol_address_size = 4,
		.command = 0x0200,
		.src_mac = stack->driver->mac.mac,
		.src_ip = stack->driver->ip.ip,
		.dest_mac = arp_resolve(stack, ip).mac,
		.dest_ip = ip.ip
	};

	etherframe_send(&stack->arp->handler, stack, arp.dest_mac, (uint8_t*) &arp,  sizeof(arp_message_t));
}

void arp_request_mac(network_stack_t* stack, ip_u ip) {
		arp_message_t arp = {
		.hardware_type = 0x0100,
		.protocol = 0x0008,
		.hardware_address_size = 6,
		.protocol_address_size = 4,
		.command = 0x0100,
		.src_mac = stack->driver->mac.mac,
		.src_ip = stack->driver->ip.ip,
		.dest_mac = 0xFFFFFFFFFFFF,
		.dest_ip = ip.ip
	};

	etherframe_send(&stack->arp->handler, stack, arp.dest_mac, (uint8_t*) &arp,  sizeof(arp_message_t));
}

mac_u arp_get_mac_from_cache(network_stack_t* stack, ip_u ip) {
		for (int i = 0; i < stack->arp->num_cache_entry; i++) {
		if (stack->arp->ip_cache[i].ip == ip.ip) {
			return stack->arp->mac_cache[i];
		}
	} 
	mac_u m = { .mac = 0xFFFFFFFFFFFF };
	return m;
}

mac_u arp_resolve(network_stack_t* stack, ip_u ip) {
	if (ip.ip == 0xffffffff) {
		mac_u m = { .mac = 0xFFFFFFFFFFFF };
		return m;
	}

	mac_u result = arp_get_mac_from_cache(stack, ip);

	if (result.mac == 0xFFFFFFFFFFFF) {
		arp_request_mac(stack, ip);
	}

	int timeout = 10000000;
	note("this isnt the best way to implement a timeout. FIXME!");
	
	while (result.mac == 0xFFFFFFFFFFFF) {
		result = arp_get_mac_from_cache(stack, ip);
		if (--timeout == 0) {
			debugf("timeout for arp request!\n");
			mac_u m = { .mac = 0 };
			return m;
		}
	}

	return result;
}

void arp_init(network_stack_t* stack) {
	stack->arp = vmm_alloc(PAGES_OF(arp_provider_t));
	memset(stack->arp, 0, sizeof(arp_provider_t));

	stack->arp->handler.ether_type_be = BSWAP16(0x806);
	stack->arp->handler.stack = stack;
	stack->arp->handler.recv = arp_etherframe_recv;
	etherframe_register(stack, stack->arp->handler);
}