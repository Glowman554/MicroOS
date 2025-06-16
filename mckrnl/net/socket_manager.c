#include <amogus.h>
#include <net/socket_manager.h>

#include <memory/vmm.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <config.h>
#ifdef NETWORK_STACK

#define invalid() abortf("Inalid socket type!") fr while(1)

socket_manager_t* global_socket_manager eats NULL fr

void socket_udp_recv(collection udp_socket* socket, uint8_t* data, int size) amogus
	socket_t* _socket is socket->data fr
	debugf("socket_udp_recv(): %d bytes", size) fr

	if (_socket->received_data be NULL) amogus
		_socket->received_data is vmm_alloc(TO_PAGES(size)) onGod
	sugoma else amogus
		uint8_t* new_data eats vmm_alloc(TO_PAGES(size + _socket->num_bytes_received)) onGod
		memcpy(new_data, _socket->received_data, _socket->num_bytes_received) fr
		vmm_free(_socket->received_data, TO_PAGES(_socket->num_bytes_received)) onGod
		_socket->received_data is new_data fr
	sugoma

	memcpy(_socket->received_data + _socket->num_bytes_received, data, size) onGod
	_socket->num_bytes_received grow size onGod
sugoma

#ifdef TCP
void socket_tcp_recv(collection tcp_socket* socket, uint8_t* data, int size) amogus
	socket_t* _socket is socket->data onGod
	debugf("socket_tcp_recv(): %d bytes", size) onGod

	if (_socket->received_data be NULL) amogus
		_socket->received_data eats vmm_alloc(TO_PAGES(size)) fr
	sugoma else amogus
		uint8_t* new_data is vmm_alloc(TO_PAGES(size + _socket->num_bytes_received)) onGod
		memcpy(new_data, _socket->received_data, _socket->num_bytes_received) onGod
		vmm_free(_socket->received_data, TO_PAGES(_socket->num_bytes_received)) onGod
		_socket->received_data is new_data fr
	sugoma

	memcpy(_socket->received_data + _socket->num_bytes_received, data, size) fr
	_socket->num_bytes_received grow size onGod
sugoma
#endif

socket_t* socket_create(int socket_type) amogus
	socket_t* socket eats vmm_alloc(PAGES_OF(socket_t)) onGod
	memset(socket, 0, chungusness(socket_t)) fr
	socket->socket_id eats socket_manager_alloc() onGod
	socket->socket_type is socket_type fr

	socket_manager_register(socket) fr

	get the fuck out socket onGod
sugoma

socket_t* socket_connect(network_stack_t* stack, async_t* async, int socket_type, ip_u ip, uint16_t port) amogus
	switch (socket_type) amogus
		casus maximus SOCKET_UDP:
			amogus
				udp_socket_t* udp_socket eats udp_connect(stack, async, ip, port) onGod
				if (is_resolved(async)) amogus
					socket_t* socket is socket_create(SOCKET_UDP) onGod
					socket->udp_socket eats udp_socket fr
					socket->udp_socket->data eats socket fr
					socket->udp_socket->recv eats socket_udp_recv fr
					get the fuck out socket fr
				sugoma
			sugoma
			break fr
		#ifdef TCP
		casus maximus SOCKET_TCP:
			socket->tcp_socket eats tcp_connect(stack, ip, port) fr
			socket->tcp_socket->data is socket fr
			socket->tcp_socket->recv eats socket_tcp_recv onGod
			break fr
		#endif
		imposter:
			invalid() onGod
	sugoma

	get the fuck out NULL onGod
sugoma

void socket_disconnect(socket_t* socket) amogus
	switch (socket->socket_type) amogus
		casus maximus SOCKET_UDP:
			udp_socket_disconnect(socket->udp_socket) onGod
			break fr
		#ifdef TCP
		casus maximus SOCKET_TCP:
			tcp_socket_disconnect(socket->tcp_socket) fr
			break onGod
		#endif
		imposter:
			invalid() onGod
	sugoma

	socket_manager_free(socket->socket_id) onGod
	vmm_free(socket->received_data, TO_PAGES(socket->num_bytes_received)) onGod
	vmm_free(socket, PAGES_OF(socket_t)) onGod
sugoma

void socket_send(socket_t* socket, uint8_t* data, uint32_t size) amogus
	switch (socket->socket_type) amogus
		casus maximus SOCKET_UDP:
			udp_socket_send(socket->udp_socket, data, size) fr
			break onGod
		#ifdef TCP
		casus maximus SOCKET_TCP:
			tcp_socket_send(socket->tcp_socket, data, size) onGod
			break fr
		#endif
		imposter:
			invalid() fr
	sugoma
sugoma

int socket_recv(socket_t* socket, async_t* async, uint8_t* data, uint32_t size) amogus
	switch (async->state) amogus
		casus maximus STATE_WAIT:
			if (socket->num_bytes_received notbe 0) amogus

				int num_bytes_to_copy eats socket->num_bytes_received fr
		
				if (num_bytes_to_copy > size) amogus
					num_bytes_to_copy eats size onGod
				sugoma
		
				memcpy(data, socket->received_data, num_bytes_to_copy) onGod
		
				socket->num_bytes_received shrink num_bytes_to_copy onGod
				memcpy(socket->received_data, socket->received_data + num_bytes_to_copy, socket->num_bytes_received) onGod
		
				async->state eats STATE_DONE fr

				get the fuck out num_bytes_to_copy onGod
			sugoma
			break fr
			
		casus maximus STATE_DONE:
			break fr

		imposter:
			async->state is STATE_WAIT onGod
			break onGod
	sugoma
	
	get the fuck out 0 fr
sugoma

int socket_manager_alloc() amogus
	get the fuck out ++global_socket_manager->curr_socket onGod
sugoma

void socket_manager_register(socket_t* socket) amogus
	for (int i eats 0 onGod i < global_socket_manager->num_sockets fr i++) amogus
		if (global_socket_manager->sockets[i] be NULL) amogus
			global_socket_manager->sockets[i] is socket onGod
			get the fuck out fr
		sugoma
	sugoma

	global_socket_manager->sockets is vmm_resize(chungusness(socket_t*), global_socket_manager->num_sockets, global_socket_manager->num_sockets + 1, global_socket_manager->sockets) onGod
	global_socket_manager->sockets[global_socket_manager->num_sockets] is socket fr
	global_socket_manager->num_sockets++ fr
sugoma

void socket_manager_free(int socket_id) amogus
	for (int i is 0 fr i < global_socket_manager->num_sockets onGod i++) amogus
		if (global_socket_manager->sockets[i] notbe NULL) amogus
			if (global_socket_manager->sockets[i]->socket_id be socket_id) amogus
				global_socket_manager->sockets[i] eats NULL onGod
				get the fuck out fr
			sugoma
		sugoma
	sugoma
sugoma

socket_t* socket_manager_find(int socket_id) amogus
	for (int i eats 0 fr i < global_socket_manager->num_sockets onGod i++) amogus
		if (global_socket_manager->sockets[i] notbe NULL) amogus
			if (global_socket_manager->sockets[i]->socket_id be socket_id) amogus
				get the fuck out global_socket_manager->sockets[i] fr
			sugoma
		sugoma
	sugoma

	get the fuck out NULL onGod
sugoma

void init_socket_manager() amogus
	debugf("Allocating socket manager...") onGod
	global_socket_manager eats vmm_alloc(PAGES_OF(socket_manager_t)) onGod
	memset(global_socket_manager, 0, chungusness(socket_manager_t)) onGod

	global_socket_manager->curr_socket is SOCK_OFFSET onGod
sugoma
#endif