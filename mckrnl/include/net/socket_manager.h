#pragma once

#include <net/stack.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <async.h>

enum socket_type_e {
	SOCKET_UDP,
	SOCKET_TCP
};

typedef struct socket {
	union {
		udp_socket_t* udp_socket;
		tcp_socket_t* tcp_socket;
	};
	int socket_type;
	int socket_id;

	int num_bytes_received;
	uint8_t* received_data;
} socket_t;

typedef struct socket_manager {
	socket_t** sockets;
	int num_sockets;
	int curr_socket;
} socket_manager_t;

extern socket_manager_t* global_socket_manager;

socket_t* socket_connect(network_stack_t* stack, async_t* async, int socket_type, ip_u ip, uint16_t port);
void socket_disconnect(socket_t* socket);
void socket_send(socket_t* socket, uint8_t* data, uint32_t size);
int socket_recv(socket_t* socket, async_t* async, uint8_t* data, uint32_t size);

void socket_udp_recv(struct udp_socket* socket, uint8_t* data, int size);

int socket_manager_alloc();
void socket_manager_register(socket_t* socket);
void socket_manager_free(int socket_id);
socket_t* socket_manager_find(int socket_id);


void init_socket_manager();