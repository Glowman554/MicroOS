#pragma once

#include <stdint.h>
#include <net/stack.h>
#include <net/etherframe.h>

typedef struct arp_message {
	uint16_t hardware_type;
	uint16_t protocol;
	uint8_t hardware_address_size;
	uint8_t protocol_address_size;
	uint16_t command;

	uint64_t src_mac: 48;
	uint32_t src_ip;
	uint64_t dest_mac: 48;
	uint32_t dest_ip;
} __attribute__((packed)) arp_message_t;

#define CACHE_SIZE 128

typedef struct arp_provider {
	ip_u ip_cache[CACHE_SIZE];
	mac_u mac_cache[CACHE_SIZE];
	int num_cache_entry;

	ether_frame_handler_t handler;
} arp_provider_t;

void arp_etherframe_recv(ether_frame_handler_t* handler, uint8_t* payload, uint32_t size);

void arp_broadcast_mac(network_stack_t* stack, ip_u ip);
void arp_request_mac(network_stack_t* stack, ip_u ip); 
mac_u arp_get_mac_from_cache(network_stack_t* stack, ip_u ip);
mac_u arp_resolve(network_stack_t* stack, ip_u ip);

void arp_init(network_stack_t* stack);
