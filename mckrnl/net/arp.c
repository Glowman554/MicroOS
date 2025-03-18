#include <driver/nic_driver.h>
#include <net/stack.h>
#include <net/arp.h>

#include <memory/vmm.h>
#include <string.h>
#include <config.h>
#ifdef NETWORK_STACK

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

void arp_broadcast_mac(network_stack_t *stack, resolvable_t* res, ip_u ip) {
	arp_resolve(stack, res, ip);
	
	if (is_resolved(res)) {
		arp_message_t arp = {
			.hardware_type = 0x0100,
			.protocol = 0x0008,
			.hardware_address_size = 6,
			.protocol_address_size = 4,
			.command = 0x0200,
			.src_mac = stack->driver->mac.mac,
			.src_ip = stack->driver->ip.ip,
			.dest_mac = cast_buffer(res, mac_u)->mac,
			.dest_ip = ip.ip
		};

		etherframe_send(&stack->arp->handler, stack, arp.dest_mac, (uint8_t*) &arp,  sizeof(arp_message_t));
	}
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
		.dest_mac = NOMAC,
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
	mac_u m = { .mac = NOMAC };
	return m;
}


void arp_resolve(network_stack_t* stack, resolvable_t* res, ip_u ip) {
	switch (res->state) {
		case STATE_INIT:
			if (ip.ip == NOIP) {
				cast_buffer(res, mac_u)->mac = NOMAC;
				res->state = STATE_DONE;
			} else {
				res->state = STATE_REQUEST;
			}
			break;
		
		case STATE_REQUEST:
			{
				mac_u result = arp_get_mac_from_cache(stack, ip);

				if (result.mac == NOMAC) {
					arp_request_mac(stack, ip);
					res->state = STATE_WAIT;
				} else {
					cast_buffer(res, mac_u)->mac = result.mac;
					res->state = STATE_DONE;
				}
			}
			break;
		
		case STATE_WAIT:
			{
				mac_u result = arp_get_mac_from_cache(stack, ip);

				if (result.mac != NOMAC) {
					cast_buffer(res, mac_u)->mac = result.mac;
					res->state = STATE_DONE;
				}
			}
			break;
		
		case STATE_DONE:
			break;

		default:
			res->state = STATE_INIT;
			break;
	}
}

void arp_init(network_stack_t* stack) {
	stack->arp = vmm_alloc(PAGES_OF(arp_provider_t));
	memset(stack->arp, 0, sizeof(arp_provider_t));

	stack->arp->handler.ether_type_be = BSWAP16(0x806);
	stack->arp->handler.stack = stack;
	stack->arp->handler.recv = arp_etherframe_recv;
	etherframe_register(stack, stack->arp->handler);
}
#endif