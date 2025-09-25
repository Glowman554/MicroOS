#pragma once

#include <net/stack.h>
#include <stdint.h>
#include <config.h>

typedef struct ether_frame_header {
	uint64_t dest_mac_be : 48;
	uint64_t src_mac_be : 48;
	uint16_t ether_type_be;
}  __attribute__((packed)) ether_frame_header_t;

typedef uint32_t ether_frame_footer_t;

typedef struct ether_frame_handler {
	uint16_t ether_type_be;
	void (*recv)(struct ether_frame_handler* handler, mac_u src_mac, uint8_t* payload, uint32_t size);
	network_stack_t* stack;
} ether_frame_handler_t;

typedef struct ether_frame_provider {
	ether_frame_handler_t* handlers;
	int num_handlers;
} ether_frame_provider_t;

void etherframe_send(ether_frame_handler_t* handler, network_stack_t* stack, uint64_t dest_mac_be, uint8_t* payload, uint32_t size);
void etherframe_register(network_stack_t* stack, ether_frame_handler_t handler);

void etherframe_nic_recv(struct nic_driver* driver, uint8_t* data, uint32_t len);

void etherframe_init(network_stack_t* stack);