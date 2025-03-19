#pragma once

#include <stdint.h>
#include <net/stack.h>
#include <net/ipv4.h>
#include <async.h>

typedef struct icmp_message {
	uint8_t type;
	uint8_t code;

	uint16_t checksum;
	uint32_t data;
} __attribute__((packed)) icmp_message_t;

typedef struct icmp_provider {
	uint32_t last_echo_reply_ip;
	ipv4_handler_t handler;
} icmp_provider_t;

void icmp_send_echo_request(network_stack_t* stack, ip_u ip, mac_u route);
void icmp_send_echo_request_and_wait(network_stack_t* stack, async_t* async, ip_u ip, mac_u route);

void icmp_ipv4_recv(struct ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size);

char* icmp_destination_unreachable_to_str(uint8_t code);

void icmp_init(network_stack_t* stack);