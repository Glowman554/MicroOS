#pragma once

#include <net/stack.h>
#include <net/ipv4.h>
#include <stdbool.h>
#include <resolvable.h>

typedef struct udp_header {
	uint16_t src_port;
	uint16_t dst_port;
	uint16_t length;
	uint16_t checksum;
} __attribute__((packed)) udp_header_t;

typedef struct udp_socket {
	uint16_t remote_port;
	uint16_t local_port;
	ip_u remote_ip;
	ip_u local_ip;

	mac_u route_mac;

	bool listening;
	network_stack_t* stack;

	void* data;

	void (*recv)(struct udp_socket* socket, uint8_t* data, int size);
} udp_socket_t;

typedef struct udp_bind {
	uint16_t port;
	udp_socket_t* socket;
} udp_bind_t;

typedef struct udp_provider {
	udp_bind_t* binds;
	int num_binds;
	int free_port;
	ipv4_handler_t handler;
} udp_provider_t;

void udp_socket_disconnect(udp_socket_t* socket);
void udp_socket_send(udp_socket_t* socket, uint8_t* data, int size);

udp_socket_t* udp_connect(network_stack_t* stack, resolvable_t* res, ip_u ip, uint16_t port);
udp_socket_t* udp_listen(network_stack_t* stack, uint16_t port);

void udp_ipv4_recv(struct ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size);

void udp_init(network_stack_t* stack);
