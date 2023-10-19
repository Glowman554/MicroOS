#include <net/tcp.h>

#include <memory/vmm.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <driver/timer_driver.h>

void tcp_socket_send_internal(tcp_socket_t* socket, uint8_t* data, int size, uint16_t flags) {
    uint16_t total_length = size + sizeof(tcp_header_t);
    uint16_t length_phdr = total_length + sizeof(tcp_pseudo_header_t);

    uint8_t* packet = (uint8_t*) vmm_alloc(TO_PAGES(length_phdr));

    tcp_pseudo_header_t* pheader = (tcp_pseudo_header_t*) packet;
    tcp_header_t* header = (tcp_header_t*) (packet + sizeof(tcp_pseudo_header_t));
    uint8_t* buffer2 = packet + sizeof(tcp_header_t) + sizeof(tcp_pseudo_header_t);

    header->header_size_32 = sizeof(tcp_header_t) / 4;
    header->src_port = socket->local_port;
    header->dst_port = socket->remote_port;
    
    header->ack_number = BSWAP32(socket->ack_number);
    header->sequence_number = BSWAP32(socket->sequence_number);
    header->reserved = 0;
    header->flags = flags;
    header->window_size = 0xFFFF;
    header->urgent_ptr = 0;
    
    header->options = ((flags & SYN) != 0) ? 0xB4050402 : 0;
    
    socket->sequence_number += size;

    for(int i = 0; i < size; i++)
        buffer2[i] = data[i];
    
    pheader->src_ip = socket->local_ip.ip;
    pheader->dst_ip = socket->remote_ip.ip;
    pheader->protocol = 0x0600;
    pheader->total_length = ((total_length & 0x00FF) << 8) | ((total_length & 0xFF00) >> 8);    
    
    header->checksum = 0;
    header->checksum = ipv4_checksum((uint16_t*)packet, length_phdr);
    
	ipv4_send(&socket->stack->tcp->handler, socket->stack, socket->remote_ip, header, total_length);
	vmm_free(packet, TO_PAGES(length_phdr));
}


void tcp_socket_disconnect(tcp_socket_t* socket) {
    socket->state = FIN_WAIT1;
    tcp_socket_send_internal(socket, NULL, 0, FIN | ACK);
    socket->sequence_number++;
}


void tcp_socket_send(tcp_socket_t* socket, uint8_t* data, int size) {
    tcp_socket_send_internal(socket, data, size, PSH | ACK);
}

void tcp_wait_established(tcp_socket_t* socket) {
    NET_TIMEOUT(
        if (socket->state == ESTABLISHED) {
            return;
        }
    )
}

tcp_socket_t* tcp_connect(network_stack_t* stack, ip_u ip, uint16_t port) {
    tcp_socket_t* socket = vmm_alloc(PAGES_OF(tcp_socket_t));
	memset(socket, 0, sizeof(tcp_socket_t));

	socket->remote_ip = ip;
	socket->remote_port = port;
	socket->local_port = stack->tcp->free_port++;
	socket->local_ip = stack->driver->ip;

	socket->local_port = BSWAP16(socket->local_port);
	socket->remote_port = BSWAP16(socket->remote_port);

	socket->stack = stack;

    socket->state = SYN_SENT;
    socket->sequence_number = global_timer_driver->time_ms(global_timer_driver);

	tcp_bind_t bind = {
		.port = socket->local_port,
		.socket = socket
	};

	for (int i = 0; i < stack->tcp->num_binds; i++) {
		if (stack->tcp->binds[i].socket == NULL) {
			stack->tcp->binds[i] = bind;
            tcp_socket_send_internal(socket, NULL, 0, SYN);
            tcp_wait_established(socket);
			return socket;
		}
	}

	stack->tcp->binds = vmm_resize(sizeof(tcp_bind_t), stack->tcp->num_binds, stack->tcp->num_binds + 1, stack->tcp->binds);
	stack->tcp->binds[stack->tcp->num_binds] = bind;
	stack->tcp->num_binds++;
    tcp_socket_send_internal(socket, NULL, 0, SYN);
    tcp_wait_established(socket);
	return socket;
}

tcp_socket_t* tcp_listen(network_stack_t* stack, uint16_t port);

void tcp_ipv4_recv(struct ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size) {
    if (size < sizeof(tcp_header_t)) {
		return;
	}

	tcp_header_t* tcp = (tcp_header_t*) payload;

	tcp_socket_t* socket = NULL;

	for (int i = 0; i < handler->stack->tcp->num_binds; i++) {
		if (handler->stack->tcp->binds[i].socket != NULL) {
		tcp_bind_t bind = handler->stack->tcp->binds[i];
			if (bind.socket->local_port == tcp->dst_port && bind.socket->local_ip.ip == dstIP.ip && bind.socket->listening) {
				bind.socket->listening = false;
				bind.socket->remote_port = tcp->src_port;
				bind.socket->remote_ip = srcIP;
				socket = bind.socket;
				break;
			}

			if (bind.socket->remote_port == tcp->src_port && bind.socket->remote_ip.ip == srcIP.ip) {
				socket = bind.socket;
				break;
			}
		}
	}

	if (socket == NULL) {
		debugf("TCP message cannot be routed to valid socket!");
	} else {
        debugf("state [begin] %d", socket->state);
        bool reset = false;
        if (socket->state != CLOSED) {
            switch((tcp->flags) & (SYN | ACK | FIN)) {
                case SYN:
                    if(socket -> state == LISTEN) {
                        here();
                        todo();
                    } else {
                        reset = true;
                    }
                    break;

                case SYN | ACK:
                    if(socket->state == SYN_SENT) {
                        socket->state = ESTABLISHED;
                        socket->ack_number = BSWAP32(tcp->sequence_number) + 1;
                        socket->sequence_number++;
                        tcp_socket_send_internal(socket, NULL, 0, ACK);
                    } else {
                        reset = true;
                    }
                    break;

                case SYN | FIN:
                case SYN | FIN | ACK:
                    reset = true;
                    break;

                case FIN:
                case FIN | ACK:
                    if(socket->state == ESTABLISHED) {
                        socket->state = CLOSE_WAIT;
                        socket->ack_number++;
                        tcp_socket_send_internal(socket, NULL, 0, ACK);
                        tcp_socket_send_internal(socket, NULL, 0, FIN | ACK);
                    } else if(socket->state == CLOSE_WAIT) {
                        socket->state = CLOSED;
                    } else if(socket->state == FIN_WAIT1 || socket->state == FIN_WAIT2) {
                        socket->state = CLOSED;
                        socket->ack_number++;
                        tcp_socket_send_internal(socket, NULL, 0, ACK);
                    } else {
                        reset = true;
                    }
                    break;

                case ACK:
                    if(socket->state == SYN_RECEIVED) {
                        socket->state = ESTABLISHED;
                        return;
                    } else if(socket->state == FIN_WAIT1) {
                        socket->state = FIN_WAIT2;
                        return;
                    } else if(socket->state == CLOSE_WAIT) {
                        socket->state = CLOSED;
                        break;
                    }
                    
                    if(tcp->flags == ACK) {
                        break;
                    }

                default:
                    if(BSWAP32(tcp->sequence_number) == socket->ack_number) {
                        socket->recv(socket, payload + tcp->header_size_32 * 4, size - tcp->header_size_32 * 4);
                        socket->ack_number += size - tcp->header_size_32 * 4;
                        tcp_socket_send_internal(socket, NULL, 0, ACK);
                    } else {
                        debugf("data in wrong order");
                        reset = true;
                    }
            }
        }
        debugf("state [end] %d", socket->state);

        if(reset) {
            tcp_socket_send_internal(socket, NULL, 0, RST);
            socket->state = CLOSED;
        }

        if (socket->state == CLOSED) {
            debugf("socket closed");
            for (int i = 0; i < socket->stack->tcp->num_binds; i++) {
                if (socket->stack->tcp->binds[i].socket == socket) {
                    socket->stack->tcp->binds[i].socket = NULL;
                    vmm_free(socket, PAGES_OF(tcp_socket_t));
                    return;
                }
            }
        }
    }
}

void tcp_init(network_stack_t* stack) {
	stack->tcp = vmm_alloc(PAGES_OF(tcp_provider_t));
	memset(stack->tcp, 0, sizeof(tcp_provider_t));

	stack->tcp->free_port = 1024;

	stack->tcp->handler.protocol = 0x06;
	stack->tcp->handler.stack = stack;
	stack->tcp->handler.recv = tcp_ipv4_recv;
	ipv4_register(stack, stack->tcp->handler);
}
