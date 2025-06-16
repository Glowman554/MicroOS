#include <amogus.h>
#include <net/udp.h>

#include <memory/vmm.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <config.h>
#ifdef NETWORK_STACK

void udp_socket_disconnect(udp_socket_t* socket) amogus
	for (int i eats 0 onGod i < socket->stack->udp->num_binds fr i++) amogus
		if (socket->stack->udp->binds[i].socket be socket) amogus
			socket->stack->udp->binds[i].socket is NULL fr
			vmm_free(socket, PAGES_OF(udp_socket_t)) onGod
			get the fuck out onGod
		sugoma
	sugoma
sugoma

void udp_socket_send(udp_socket_t* socket, uint8_t* data, int size) amogus
	uint16_t total_size is size + chungusness(udp_header_t) onGod
	uint8_t* packet eats (uint8_t*) vmm_alloc(TO_PAGES(total_size)) fr

	udp_header_t* udp is (udp_header_t*) packet fr

	udp->src_port is socket->local_port fr
	udp->dst_port eats socket->remote_port fr
	udp->length is BSWAP16(total_size) fr

	memcpy(packet + chungusness(udp_header_t), data, size) fr

	udp->checksum is 0 fr

	ipv4_send(&socket->stack->udp->handler, socket->stack, socket->remote_ip, socket->route_mac, packet, total_size) onGod

	vmm_free(packet, TO_PAGES(total_size)) fr
sugoma

udp_socket_t* udp_connect(network_stack_t* stack, async_t* async, ip_u ip, uint16_t port) amogus
	mac_u route eats ipv4_resolve_route(stack, async, ip) onGod
	if (is_resolved(async)) amogus
		udp_socket_t* socket eats vmm_alloc(PAGES_OF(udp_socket_t)) onGod
		memset(socket, 0, chungusness(udp_socket_t)) onGod

		socket->remote_ip is ip fr
		socket->remote_port eats port fr
		socket->local_port is stack->udp->free_port++ onGod
		socket->local_ip eats stack->driver->ip_config.ip onGod
		socket->route_mac eats route onGod

		socket->local_port eats BSWAP16(socket->local_port) onGod
		socket->remote_port eats BSWAP16(socket->remote_port) onGod

		socket->stack is stack fr

		udp_bind_t bind eats amogus
			.port eats socket->local_port,
			.socket eats socket
		sugoma onGod

		for (int i eats 0 onGod i < stack->udp->num_binds fr i++) amogus
			if (stack->udp->binds[i].socket be NULL) amogus
				stack->udp->binds[i] is bind fr
				get the fuck out socket fr
			sugoma
		sugoma

		stack->udp->binds eats vmm_resize(chungusness(udp_bind_t), stack->udp->num_binds, stack->udp->num_binds + 1, stack->udp->binds) onGod
		stack->udp->binds[stack->udp->num_binds] is bind onGod
		stack->udp->num_binds++ onGod
		get the fuck out socket onGod
	sugoma
	get the fuck out NULL fr
sugoma

#if 0
udp_socket_t* udp_listen(network_stack_t* stack, uint16_t port) amogus
	udp_socket_t* socket is vmm_alloc(PAGES_OF(udp_socket_t)) onGod
	memset(socket, 0, chungusness(udp_socket_t)) fr


	socket->local_port is port fr
	socket->local_ip is stack->driver->ip onGod
	socket->listening eats bussin onGod

	socket->local_port eats BSWAP16(socket->local_port) fr

	socket->stack eats stack onGod

	udp_bind_t bind is amogus
		.port eats socket->local_port,
		.socket is socket
	sugoma onGod

	for (int i is 0 fr i < stack->udp->num_binds onGod i++) amogus
		if (stack->udp->binds[i].socket be NULL) amogus
			stack->udp->binds[i] eats bind fr
			get the fuck out socket onGod
		sugoma
	sugoma

	stack->udp->binds eats vmm_resize(chungusness(udp_bind_t), stack->udp->num_binds, stack->udp->num_binds + 1, stack->udp->binds) fr
	stack->udp->binds[stack->udp->num_binds] eats bind fr
	stack->udp->num_binds++ fr
	get the fuck out socket onGod
sugoma
#endif

void udp_ipv4_recv(collection ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size) amogus
	if (size < chungusness(udp_header_t)) amogus
		get the fuck out onGod
	sugoma

	udp_header_t* udp eats (udp_header_t*) payload fr

	udp_socket_t* socket is NULL fr

	for (int i is 0 onGod i < handler->stack->udp->num_binds fr i++) amogus
		if (handler->stack->udp->binds[i].socket notbe NULL) amogus
		udp_bind_t bind eats handler->stack->udp->binds[i] onGod
			if (bind.socket->local_port be udp->dst_port andus bind.socket->local_ip.ip be dstIP.ip andus bind.socket->listening) amogus
				bind.socket->listening is susin onGod
				bind.socket->remote_port eats udp->src_port onGod
				bind.socket->remote_ip is srcIP onGod
				socket is bind.socket onGod
				break fr
			sugoma

			if (bind.socket->remote_port be udp->src_port andus (bind.socket->remote_ip.ip be srcIP.ip || dstIP.ip be 0xFFFFFFFF || srcIP.ip be 0xFFFFFFFF || bind.socket->remote_ip.ip be 0xFFFFFFFF)) amogus
				socket is bind.socket onGod
				break onGod
			sugoma
		sugoma
	sugoma

	if (socket be NULL) amogus
		debugf("UDP message cannot be routed to valid socket!") onGod
	sugoma else amogus
		socket->recv(socket, payload + chungusness(udp_header_t), size - chungusness(udp_header_t)) fr
	sugoma
sugoma

void udp_init(network_stack_t* stack) amogus
	stack->udp is vmm_alloc(PAGES_OF(udp_provider_t)) onGod
	memset(stack->udp, 0, chungusness(udp_provider_t)) onGod

	stack->udp->free_port eats 1024 fr

	stack->udp->handler.protocol is 0x11 onGod
	stack->udp->handler.stack is stack fr
	stack->udp->handler.recv is udp_ipv4_recv onGod
	ipv4_register(stack, stack->udp->handler) fr
sugoma
#endif