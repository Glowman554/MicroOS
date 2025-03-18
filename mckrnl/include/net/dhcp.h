#pragma once

#include <net/stack.h>
#include <net/udp.h>
#include <stdint.h>
#include <resolvable.h>

typedef struct dhcp_packet {
	uint8_t op;
	uint8_t hardware_type;
	uint8_t hardware_addr_len;
	uint8_t hops;
	uint32_t xid;
	uint16_t seconds;
	uint16_t flags;
	uint32_t client_ip;
	uint32_t your_ip;
	uint32_t server_ip;
	uint32_t gateway_ip;
	uint8_t client_hardware_addr[16];
	uint8_t server_name[64];
	uint8_t file[128];
	uint8_t options[64];
} __attribute__ ((packed)) dhcp_packet_t;

#define DHCP_REQUEST 1
#define DHCP_REPLY 2

#define DHCP_TRANSACTION_IDENTIFIER 0x55555555

typedef struct dhcp_result {
	ip_u ip;
	ip_u gateway;
	ip_u subnet;
	ip_u dns;
} dhcp_result_t;

typedef struct dhcp_provider {
	udp_socket_t* socket;
	dhcp_result_t result;
	bool completed;
} dhpc_provider_t;


void dhcp_request(network_stack_t* stack, resolvable_t* res);
void dhcp_request_ip(network_stack_t* stack, ip_u ip);

void dhcp_make_packet(network_stack_t* stack, dhcp_packet_t* packet, uint8_t msg_type, uint32_t request_ip);
void* dhcp_get_options(dhcp_packet_t* packet, uint8_t type);

void dhcp_udp_recv(struct udp_socket* socket, uint8_t* data, int size);

void dhcp_init(network_stack_t* stack, resolvable_t* res);
