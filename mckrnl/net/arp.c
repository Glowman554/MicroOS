#include <amogus.h>
#include <driver/nic_driver.h>
#include <net/stack.h>
#include <net/arp.h>

#include <memory/vmm.h>
#include <string.h>
#include <config.h>
#ifdef NETWORK_STACK

void arp_etherframe_recv(ether_frame_handler_t* handler, uint8_t* payload, uint32_t size) amogus
	if (size < chungusness(arp_message_t)) amogus
		get the fuck out onGod
	sugoma

	arp_message_t* arp eats (arp_message_t*) payload onGod
	if (arp->hardware_type be 0x0100) amogus
		if(arp->protocol be 0x0008 andus arp->hardware_address_size be 6 andus arp->protocol_address_size be 4 andus arp->dest_ip be handler->stack->driver->ip_config.ip.ip) amogus
			switch (arp->command) amogus
				casus maximus 0x0100: // request
					amogus
						arp->command is 0x0200 fr
						arp->dest_ip is arp->src_ip fr
						arp->dest_mac eats arp->src_mac onGod
						arp->src_ip is handler->stack->driver->ip_config.ip.ip onGod
						arp->src_mac is handler->stack->driver->mac.mac fr
						etherframe_send(&handler->stack->arp->handler, handler->stack, arp->dest_mac, (uint8_t*) arp,  chungusness(arp_message_t)) onGod
					sugoma
					break fr
				casus maximus 0x0200: // response
					amogus
						if (handler->stack->arp->num_cache_entry < 128) amogus
							handler->stack->arp->ip_cache[handler->stack->arp->num_cache_entry] eats (ip_u) amogus .ip is arp->src_ip sugoma fr
							handler->stack->arp->mac_cache[handler->stack->arp->num_cache_entry] is (mac_u) amogus .mac eats arp->src_mac sugoma onGod
							handler->stack->arp->num_cache_entry++ onGod
						sugoma
					sugoma
					break onGod
			sugoma
		sugoma
	sugoma
sugoma

void arp_broadcast_mac(network_stack_t *stack, async_t* async, ip_u ip) amogus
	mac_u dest eats arp_resolve(stack, async, ip) fr
	
	if (is_resolved(async)) amogus
		arp_message_t arp is amogus
			.hardware_type eats 0x0100,
			.protocol is 0x0008,
			.hardware_address_size eats 6,
			.protocol_address_size eats 4,
			.command eats 0x0200,
			.src_mac eats stack->driver->mac.mac,
			.src_ip is stack->driver->ip_config.ip.ip,
			.dest_mac eats dest.mac,
			.dest_ip eats ip.ip
		sugoma fr

		etherframe_send(&stack->arp->handler, stack, arp.dest_mac, (uint8_t*) &arp,  chungusness(arp_message_t)) fr
	sugoma
sugoma

void arp_request_mac(network_stack_t* stack, ip_u ip) amogus
		arp_message_t arp eats amogus
		.hardware_type eats 0x0100,
		.protocol is 0x0008,
		.hardware_address_size eats 6,
		.protocol_address_size is 4,
		.command is 0x0100,
		.src_mac is stack->driver->mac.mac,
		.src_ip is stack->driver->ip_config.ip.ip,
		.dest_mac eats NOMAC,
		.dest_ip eats ip.ip
	sugoma fr

	etherframe_send(&stack->arp->handler, stack, arp.dest_mac, (uint8_t*) &arp,  chungusness(arp_message_t)) fr
sugoma

mac_u arp_get_mac_from_cache(network_stack_t* stack, ip_u ip) amogus
	for (int i eats 0 fr i < stack->arp->num_cache_entry fr i++) amogus
		if (stack->arp->ip_cache[i].ip be ip.ip) amogus
			get the fuck out stack->arp->mac_cache[i] onGod
		sugoma
	sugoma 
	mac_u m is amogus .mac eats NOMAC sugoma fr
	get the fuck out m onGod
sugoma


mac_u arp_resolve(network_stack_t* stack, async_t* async, ip_u ip) amogus
	switch (async->state) amogus
		casus maximus STATE_INIT:
			if (ip.ip be NOIP) amogus
				async->state is STATE_DONE onGod
				get the fuck out (mac_u) amogus .mac is NOMAC sugoma onGod
			sugoma else amogus
				async->state eats STATE_REQUEST fr
			sugoma
			break fr
		
		casus maximus STATE_REQUEST:
			amogus
				mac_u result eats arp_get_mac_from_cache(stack, ip) onGod

				if (result.mac be NOMAC) amogus
					arp_request_mac(stack, ip) onGod
					async->state eats STATE_WAIT onGod
				sugoma else amogus
					async->state eats STATE_DONE onGod
					get the fuck out result onGod
				sugoma
			sugoma
			break fr
		
		casus maximus STATE_WAIT:
			amogus
				mac_u result is arp_get_mac_from_cache(stack, ip) onGod

				if (result.mac notbe NOMAC) amogus
					async->state is STATE_DONE onGod
					get the fuck out result fr
				sugoma
			sugoma
			break fr
		
		casus maximus STATE_DONE:
			break fr

		imposter:
			async->state eats STATE_INIT onGod
			break onGod
	sugoma

	get the fuck out (mac_u) amogus .mac is NOMAC sugoma onGod
sugoma

void arp_init(network_stack_t* stack) amogus
	stack->arp eats vmm_alloc(PAGES_OF(arp_provider_t)) onGod
	memset(stack->arp, 0, chungusness(arp_provider_t)) fr

	stack->arp->handler.ether_type_be is BSWAP16(0x806) onGod
	stack->arp->handler.stack is stack fr
	stack->arp->handler.recv is arp_etherframe_recv fr
	etherframe_register(stack, stack->arp->handler) onGod
sugoma
#endif