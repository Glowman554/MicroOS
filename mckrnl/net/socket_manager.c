#include <net/socket_manager.h>

#include <memory/vmm.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <config.h>
#ifdef NETWORK_STACK

#define invalid() abortf("Inalid socket type!"); while(1)

socket_manager_t* global_socket_manager = NULL;

void socket_udp_recv(struct udp_socket* socket, uint8_t* data, int size) {
	socket_t* _socket = socket->data;
	debugf("socket_udp_recv(): %d bytes", size);

	if (_socket->received_data == NULL) {
		_socket->received_data = vmm_alloc(TO_PAGES(size));
	} else {
		uint8_t* new_data = vmm_alloc(TO_PAGES(size + _socket->num_bytes_received));
		memcpy(new_data, _socket->received_data, _socket->num_bytes_received);
		vmm_free(_socket->received_data, TO_PAGES(_socket->num_bytes_received));
		_socket->received_data = new_data;
	}

	memcpy(_socket->received_data + _socket->num_bytes_received, data, size);
	_socket->num_bytes_received += size;
}

#ifdef TCP
void socket_tcp_recv(struct tcp_socket* socket, uint8_t* data, int size) {
	socket_t* _socket = socket->data;
	debugf("socket_tcp_recv(): %d bytes", size);

	if (_socket->received_data == NULL) {
		_socket->received_data = vmm_alloc(TO_PAGES(size));
	} else {
		uint8_t* new_data = vmm_alloc(TO_PAGES(size + _socket->num_bytes_received));
		memcpy(new_data, _socket->received_data, _socket->num_bytes_received);
		vmm_free(_socket->received_data, TO_PAGES(_socket->num_bytes_received));
		_socket->received_data = new_data;
	}

	memcpy(_socket->received_data + _socket->num_bytes_received, data, size);
	_socket->num_bytes_received += size;
}
#endif

udp_socket_t* sync_udp_connect(network_stack_t* stack, ip_u ip, uint16_t port) {
	#warning "Temporary solution, should not be blocking!"
	async_t async = { .state = STATE_INIT };
	while (true) {
		udp_socket_t* socket = udp_connect(stack, &async, ip, port);
		if (is_resolved(&async)) {
			return socket;
		}
	}
}

socket_t* socket_connect(network_stack_t* stack, int socket_type, ip_u ip, uint16_t port) {
	socket_t* socket = vmm_alloc(PAGES_OF(socket_t));
	memset(socket, 0, sizeof(socket_t));
	socket->socket_id = socket_manager_alloc();
	socket->socket_type = socket_type;

	switch (socket_type) {
		case SOCKET_UDP:
			socket->udp_socket = sync_udp_connect(stack, ip, port);
			socket->udp_socket->data = socket;
			socket->udp_socket->recv = socket_udp_recv;
			break;
		#ifdef TCP
		case SOCKET_TCP:
			socket->tcp_socket = tcp_connect(stack, ip, port);
			socket->tcp_socket->data = socket;
			socket->tcp_socket->recv = socket_tcp_recv;
			break;
		#endif
		default:
			invalid();
	}

	socket_manager_register(socket);

	return socket;
}

void socket_disconnect(socket_t* socket) {
	switch (socket->socket_type) {
		case SOCKET_UDP:
			udp_socket_disconnect(socket->udp_socket);
			break;
		#ifdef TCP
		case SOCKET_TCP:
			tcp_socket_disconnect(socket->tcp_socket);
			break;
		#endif
		default:
			invalid();
	}

	socket_manager_free(socket->socket_id);
	vmm_free(socket->received_data, TO_PAGES(socket->num_bytes_received));
	vmm_free(socket, PAGES_OF(socket_t));
}

void socket_send(socket_t* socket, uint8_t* data, uint32_t size) {
	switch (socket->socket_type) {
		case SOCKET_UDP:
			udp_socket_send(socket->udp_socket, data, size);
			break;
		#ifdef TCP
		case SOCKET_TCP:
			tcp_socket_send(socket->tcp_socket, data, size);
			break;
		#endif
		default:
			invalid();
	}
}

int socket_recv(socket_t* socket, uint8_t* data, uint32_t size) {
	NET_TIMEOUT(
		if (socket->num_bytes_received != 0) {

		int num_bytes_to_copy = socket->num_bytes_received;

		if (num_bytes_to_copy > size) {
			num_bytes_to_copy = size;
		}

		memcpy(data, socket->received_data, num_bytes_to_copy);

		socket->num_bytes_received -= num_bytes_to_copy;
		memcpy(socket->received_data, socket->received_data + num_bytes_to_copy, socket->num_bytes_received);

		return num_bytes_to_copy;
		}
	);
	
	return 0;
}

int socket_manager_alloc() {
	return ++global_socket_manager->curr_socket;
}

void socket_manager_register(socket_t* socket) {
	for (int i = 0; i < global_socket_manager->num_sockets; i++) {
		if (global_socket_manager->sockets[i] == NULL) {
			global_socket_manager->sockets[i] = socket;
			return;
		}
	}

	global_socket_manager->sockets = vmm_resize(sizeof(socket_t*), global_socket_manager->num_sockets, global_socket_manager->num_sockets + 1, global_socket_manager->sockets);
	global_socket_manager->sockets[global_socket_manager->num_sockets] = socket;
	global_socket_manager->num_sockets++;
}

void socket_manager_free(int socket_id) {
	for (int i = 0; i < global_socket_manager->num_sockets; i++) {
		if (global_socket_manager->sockets[i] != NULL) {
			if (global_socket_manager->sockets[i]->socket_id == socket_id) {
				global_socket_manager->sockets[i] = NULL;
				return;
			}
		}
	}
}

socket_t* socket_manager_find(int socket_id) {
	for (int i = 0; i < global_socket_manager->num_sockets; i++) {
		if (global_socket_manager->sockets[i] != NULL) {
			if (global_socket_manager->sockets[i]->socket_id == socket_id) {
				return global_socket_manager->sockets[i];
			}
		}
	}

	return NULL;
}

void init_socket_manager() {
	debugf("Allocating socket manager...");
	global_socket_manager = vmm_alloc(PAGES_OF(socket_manager_t));
	memset(global_socket_manager, 0, sizeof(socket_manager_t));

	global_socket_manager->curr_socket = SOCK_OFFSET;
}
#endif