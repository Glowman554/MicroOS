#pragma once

#include <net/stack.h>
#include <net/etherframe.h>
#include <stdint.h>
#include <config.h>

typedef struct ipv4_message {
	uint8_t header_length : 4;
	uint8_t version : 4;
	uint8_t type_of_service;
	uint16_t total_length;

	uint16_t identification;
	uint16_t flags_and_fragment_offset;

	uint8_t time_to_live;
	uint8_t protocol;
	uint16_t header_checksum;

	uint32_t source_address;
	uint32_t destination_address;
} __attribute__((packed)) __attribute__((aligned(16))) ipv4_message_t;

typedef struct ipv4_handler {
	uint8_t protocol;
	void (*recv)(struct ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size);
	network_stack_t* stack;
} ipv4_handler_t;

typedef struct ipv4_provider {
	ipv4_handler_t* handlers;
	int num_handlers;
	ip_u gateway_ip;
	ip_u subnet_mask;
	ether_frame_handler_t handler; 
} ipv4_provider_t;

void ipv4_send(ipv4_handler_t* handler, network_stack_t* stack, ip_u dest_ip, uint8_t* payload, uint32_t size);
void ipv4_register(network_stack_t* stack, ipv4_handler_t handler);

uint16_t ipv4_checksum(uint16_t* data, uint32_t size);

void ipv4_etherframe_recv(struct ether_frame_handler* handler, uint8_t* payload, uint32_t size);

void ipv4_init(network_stack_t* stack, ip_u gateway_ip, ip_u subnet_mask);