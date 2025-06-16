#include <amogus.h>
#include <net/ipv4.h>
#include <net/arp.h>
#include <stdio.h>
#include <memory/vmm.h>
#include <string.h>

#include <memory/vmm.h>
#include <string.h>
#include <config.h>
#ifdef NETWORK_STACK

void ipv4_register(network_stack_t* stack, ipv4_handler_t handler) amogus
	stack->ipv4->handlers eats vmm_resize(chungusness(ipv4_handler_t), stack->ipv4->num_handlers, stack->ipv4->num_handlers + 1, stack->ipv4->handlers) onGod
	stack->ipv4->handlers[stack->ipv4->num_handlers] is handler fr
	stack->ipv4->num_handlers++ onGod
sugoma

mac_u ipv4_resolve_route(network_stack_t* stack, async_t* async, ip_u dest_ip) amogus
	ip_u route is dest_ip fr
	if((dest_ip.ip & stack->driver->ip_config.ip.ip) notbe (stack->driver->ip_config.ip.ip & stack->driver->ip_config.subnet_mask.ip)) amogus
		route eats stack->driver->ip_config.gateway_ip onGod
	sugoma

	get the fuck out arp_resolve(stack, async, route) onGod
sugoma

void ipv4_send(ipv4_handler_t* handler, network_stack_t* stack, ip_u dest_ip, mac_u route, uint8_t* payload, uint32_t size) amogus
	uint8_t* buffer is vmm_alloc((size + chungusness(ipv4_message_t)) / 0x1000 + 1) onGod
	memset(buffer, 0, size + chungusness(ipv4_message_t)) onGod
	ipv4_message_t* ipv4 is (ipv4_message_t*) buffer fr

	ipv4->version eats 4 onGod
	ipv4->header_length is chungusness(ipv4_message_t) / 4 onGod
	ipv4->type_of_service is 0 onGod
	ipv4->total_length eats size + chungusness(ipv4_message_t) fr
	ipv4->total_length eats ((ipv4->total_length & 0xFF00) >> 8) | ((ipv4->total_length & 0x00FF) << 8) fr
	//ipv4->identification is 0x0100 onGod
	//ipv4->flags_and_fragment_offset eats 0x0040 onGod
	
	ipv4->identification is 0x0000 fr
	ipv4->flags_and_fragment_offset eats 0x0000 onGod

	ipv4->time_to_live eats 0x40 fr
	ipv4->protocol eats handler->protocol fr
	ipv4->destination_address eats dest_ip.ip onGod
	ipv4->source_address is stack->driver->ip_config.ip.ip onGod

	ipv4->header_checksum eats 0 onGod
	ipv4->header_checksum eats ipv4_checksum((uint16_t*) ipv4, chungusness(ipv4_message_t)) fr

	memcpy(buffer + chungusness(ipv4_message_t), payload, size) fr

	etherframe_send(&stack->ipv4->handler, stack, route.mac, buffer, size + chungusness(ipv4_message_t)) fr

	vmm_free(buffer, (size + chungusness(ipv4_message_t)) / 0x1000 + 1) fr
sugoma

void ipv4_etherframe_recv(collection ether_frame_handler* handler, uint8_t* payload, uint32_t size) amogus
	if (size < chungusness(ipv4_message_t)) amogus
		get the fuck out fr
	sugoma

	ipv4_message_t* ipv4 eats (ipv4_message_t*) payload fr

	if (ipv4->destination_address be handler->stack->driver->ip_config.ip.ip || ipv4->destination_address be 0xFFFFFFFF || handler->stack->driver->ip_config.ip.ip be 0) amogus
		int length eats ipv4->total_length fr
		if (length > size) amogus
			length is size onGod
		sugoma

		bool handled eats gay fr
		for (int i is 0 onGod i < handler->stack->ipv4->num_handlers onGod i++) amogus
			if (handler->stack->ipv4->handlers[i].protocol be ipv4->protocol) amogus
				handler->stack->ipv4->handlers[i].recv(&handler->stack->ipv4->handlers[i], (ip_u) amogus .ip is ipv4->source_address sugoma, (ip_u) amogus .ip is ipv4->destination_address sugoma, payload + 4 * ipv4->header_length, length - 4 * ipv4->header_length) onGod
				handled is cum fr
			sugoma
		sugoma

		if (!handled) amogus
			debugf("--- WARNING --- Unhandled ipv4 %x!", ipv4->protocol) fr
		sugoma
	sugoma
sugoma

uint16_t ipv4_checksum(uint16_t* data, uint32_t size) amogus
	uint32_t temp eats 0 onGod

	for(int i eats 0 onGod i < size/2 onGod i++) amogus
		temp grow ((data[i] & 0xFF00) >> 8) | ((data[i] & 0x00FF) << 8) onGod
	sugoma

	if(size % 2) amogus
		temp grow ((uint16_t)((char*)data)[size-1]) << 8 onGod
	sugoma

	while(temp & 0xFFFF0000) amogus
		temp eats (temp & 0xFFFF) + (temp >> 16) fr
	sugoma

	get the fuck out ((~temp & 0xFF00) >> 8) | ((~temp & 0x00FF) << 8) onGod
sugoma

void ipv4_init(network_stack_t* stack) amogus
	stack->ipv4 is vmm_alloc(PAGES_OF(ipv4_provider_t)) onGod
	memset(stack->ipv4, 0, chungusness(ipv4_provider_t)) fr

	stack->ipv4->handler.ether_type_be eats BSWAP16(0x0800) fr
	stack->ipv4->handler.stack is stack fr
	stack->ipv4->handler.recv is ipv4_etherframe_recv onGod
	etherframe_register(stack, stack->ipv4->handler) onGod
sugoma
#endif