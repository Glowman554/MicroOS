#include <net/ipv4.h>
#include <net/arp.h>
#include <stdio.h>
#include <memory/vmm.h>
#include <string.h>

#include <memory/vmm.h>
#include <string.h>
#include <config.h>
#ifdef NETWORK_STACK

void ipv4_register(network_stack_t* stack, ipv4_handler_t handler) {
	stack->ipv4->handlers = vmm_resize(sizeof(ipv4_handler_t), stack->ipv4->num_handlers, stack->ipv4->num_handlers + 1, stack->ipv4->handlers);
	stack->ipv4->handlers[stack->ipv4->num_handlers] = handler;
	stack->ipv4->num_handlers++;
}

mac_u ipv4_resolve_route(network_stack_t* stack, async_t* async, ip_u dest_ip) {
	ip_u route = dest_ip;
	if((dest_ip.ip & stack->driver->ip_config.ip.ip) != (stack->driver->ip_config.ip.ip & stack->driver->ip_config.subnet_mask.ip)) {
		route = stack->driver->ip_config.gateway_ip;
	}

	return arp_resolve(stack, async, route);
}

void ipv4_send(ipv4_handler_t* handler, network_stack_t* stack, ip_u dest_ip, mac_u route, uint8_t* payload, uint32_t size) {
	uint8_t* buffer = vmm_alloc((size + sizeof(ipv4_message_t)) / 0x1000 + 1);
	memset(buffer, 0, size + sizeof(ipv4_message_t));
	ipv4_message_t* ipv4 = (ipv4_message_t*) buffer;

	ipv4->version = 4;
	ipv4->header_length = sizeof(ipv4_message_t) / 4;
	ipv4->type_of_service = 0;
	ipv4->total_length = size + sizeof(ipv4_message_t);
	ipv4->total_length = ((ipv4->total_length & 0xFF00) >> 8) | ((ipv4->total_length & 0x00FF) << 8);
	//ipv4->identification = 0x0100;
	//ipv4->flags_and_fragment_offset = 0x0040;
	
	ipv4->identification = 0x0000;
	ipv4->flags_and_fragment_offset = 0x0000;

	ipv4->time_to_live = 0x40;
	ipv4->protocol = handler->protocol;
	ipv4->destination_address = dest_ip.ip;
	ipv4->source_address = stack->driver->ip_config.ip.ip;

	ipv4->header_checksum = 0;
	ipv4->header_checksum = ipv4_checksum((uint16_t*) ipv4, sizeof(ipv4_message_t));

	memcpy(buffer + sizeof(ipv4_message_t), payload, size);

	etherframe_send(&stack->ipv4->handler, stack, route.mac, buffer, size + sizeof(ipv4_message_t));

	vmm_free(buffer, (size + sizeof(ipv4_message_t)) / 0x1000 + 1);
}

void ipv4_etherframe_recv(struct ether_frame_handler* handler, uint8_t* payload, uint32_t size) {
	if (size < sizeof(ipv4_message_t)) {
		return;
	}

	ipv4_message_t* ipv4 = (ipv4_message_t*) payload;

	if (ipv4->destination_address == handler->stack->driver->ip_config.ip.ip || ipv4->destination_address == 0xFFFFFFFF || handler->stack->driver->ip_config.ip.ip == 0) {
		int length = ipv4->total_length;
		if (length > size) {
			length = size;
		}

		bool handled = false;
		for (int i = 0; i < handler->stack->ipv4->num_handlers; i++) {
			if (handler->stack->ipv4->handlers[i].protocol == ipv4->protocol) {
				handler->stack->ipv4->handlers[i].recv(&handler->stack->ipv4->handlers[i], (ip_u) { .ip = ipv4->source_address }, (ip_u) { .ip = ipv4->destination_address }, payload + 4 * ipv4->header_length, length - 4 * ipv4->header_length);
				handled = true;
			}
		}

		if (!handled) {
			debugf("--- WARNING --- Unhandled ipv4 %x!", ipv4->protocol);
		}
	}
}

uint16_t ipv4_checksum(uint16_t* data, uint32_t size) {
	uint32_t temp = 0;

	for(int i = 0; i < size/2; i++) {
		temp += ((data[i] & 0xFF00) >> 8) | ((data[i] & 0x00FF) << 8);
	}

	if(size % 2) {
		temp += ((uint16_t)((char*)data)[size-1]) << 8;
	}

	while(temp & 0xFFFF0000) {
		temp = (temp & 0xFFFF) + (temp >> 16);
	}

	return ((~temp & 0xFF00) >> 8) | ((~temp & 0x00FF) << 8);
}

void ipv4_init(network_stack_t* stack) {
	stack->ipv4 = vmm_alloc(PAGES_OF(ipv4_provider_t));
	memset(stack->ipv4, 0, sizeof(ipv4_provider_t));

	stack->ipv4->handler.ether_type_be = BSWAP16(0x0800);
	stack->ipv4->handler.stack = stack;
	stack->ipv4->handler.recv = ipv4_etherframe_recv;
	etherframe_register(stack, stack->ipv4->handler);
}
#endif