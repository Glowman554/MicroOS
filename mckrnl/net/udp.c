#include <net/udp.h>

#include <memory/vmm.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

void udp_socket_disconnect(udp_socket_t* socket) {
	for (int i = 0; i < socket->stack->udp->num_binds; i++) {
		if (socket->stack->udp->binds[i].socket == socket) {
			socket->stack->udp->binds[i].socket = NULL;
			vmm_free(socket, PAGES_OF(udp_socket_t));
			return;
		}
	}
}

void udp_socket_send(udp_socket_t* socket, uint8_t* data, int size) {
	uint16_t total_size = size + sizeof(udp_header_t);
	uint8_t* packet = (uint8_t*) vmm_alloc(TO_PAGES(total_size));

	udp_header_t* udp = (udp_header_t*) packet;

	udp->src_port = socket->local_port;
	udp->dst_port = socket->remote_port;
	udp->length = BSWAP16(total_size);

	memcpy(packet + sizeof(udp_header_t), data, size);

	udp->checksum = 0;

	ipv4_send(&socket->stack->udp->handler, socket->stack, socket->remote_ip, packet, total_size);

	vmm_free(packet, TO_PAGES(total_size));
}

udp_socket_t* udp_connect(network_stack_t* stack, ip_u ip, uint16_t port) {
	udp_socket_t* socket = vmm_alloc(PAGES_OF(udp_socket_t));
	memset(socket, 0, sizeof(udp_socket_t));

	socket->remote_ip = ip;
	socket->remote_port = port;
	socket->local_port = stack->udp->free_port++;
	socket->local_ip = stack->driver->ip;

	socket->local_port = BSWAP16(socket->local_port);
	socket->remote_port = BSWAP16(socket->remote_port);

	socket->stack = stack;

	udp_bind_t bind = {
		.port = socket->local_port,
		.socket = socket
	};

	for (int i = 0; i < stack->udp->num_binds; i++) {
		if (stack->udp->binds[i].socket == NULL) {
			stack->udp->binds[i] = bind;
			return socket;
		}
	}

	stack->udp->binds = vmm_resize(sizeof(udp_bind_t), stack->udp->num_binds, stack->udp->num_binds + 1, stack->udp->binds);
	stack->udp->binds[stack->udp->num_binds] = bind;
	stack->udp->num_binds++;
	return socket;
}

udp_socket_t* udp_listen(network_stack_t* stack, uint16_t port) {
	udp_socket_t* socket = vmm_alloc(PAGES_OF(udp_socket_t));
	memset(socket, 0, sizeof(udp_socket_t));


	socket->local_port = port;
	socket->local_ip = stack->driver->ip;
	socket->listening = true;

	socket->local_port = BSWAP16(socket->local_port);

	socket->stack = stack;

	udp_bind_t bind = {
		.port = socket->local_port,
		.socket = socket
	};

	for (int i = 0; i < stack->udp->num_binds; i++) {
		if (stack->udp->binds[i].socket == NULL) {
			stack->udp->binds[i] = bind;
			return socket;
		}
	}

	stack->udp->binds = vmm_resize(sizeof(udp_bind_t), stack->udp->num_binds, stack->udp->num_binds + 1, stack->udp->binds);
	stack->udp->binds[stack->udp->num_binds] = bind;
	stack->udp->num_binds++;
	return socket;
}

void udp_ipv4_recv(struct ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size) {
	if (size < sizeof(udp_header_t)) {
		return;
	}

	udp_header_t* udp = (udp_header_t*) payload;

	udp_socket_t* socket = NULL;

	for (int i = 0; i < handler->stack->udp->num_binds; i++) {
		if (handler->stack->udp->binds[i].socket != NULL) {
		udp_bind_t bind = handler->stack->udp->binds[i];
			if (bind.socket->local_port == udp->dst_port && bind.socket->local_ip.ip == dstIP.ip && bind.socket->listening) {
				bind.socket->listening = false;
				bind.socket->remote_port = udp->src_port;
				bind.socket->remote_ip = srcIP;
				socket = bind.socket;
				break;
			}

			if (bind.socket->remote_port == udp->src_port && (bind.socket->remote_ip.ip == srcIP.ip || dstIP.ip == 0xFFFFFFFF || srcIP.ip == 0xFFFFFFFF || bind.socket->remote_ip.ip == 0xFFFFFFFF)) {
				socket = bind.socket;
				break;
			}
		}
	}

	if (socket == NULL) {
		debugf("UDP message cannot be routed to valid socket!");
	} else {
		socket->recv(socket, payload + sizeof(udp_header_t), size - sizeof(udp_header_t));
	}
}

void udp_init(network_stack_t* stack) {
	stack->udp = vmm_alloc(PAGES_OF(udp_provider_t));
	memset(stack->udp, 0, sizeof(udp_provider_t));

	stack->udp->free_port = 1024;

	stack->udp->handler.protocol = 0x11;
	stack->udp->handler.stack = stack;
	stack->udp->handler.recv = udp_ipv4_recv;
	ipv4_register(stack, stack->udp->handler);
}
