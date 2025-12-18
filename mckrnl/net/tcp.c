#include <net/tcp.h>

#include <memory/vmm.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <driver/timer_driver.h>
#include <config.h>
#ifdef NETWORK_STACK

const char* tcp_socket_state_str[] = {
    [CLOSED] = "CLOSED",
    [LISTEN] = "LISTEN",
    [SYN_SENT] = "SYN_SENT",
    [SYN_RECEIVED] = "SYN_RECEIVED",
    [ESTABLISHED] = "ESTABLISHED",
    [FIN_WAIT1] = "FIN_WAIT1",
    [FIN_WAIT2] = "FIN_WAIT2",
    [CLOSING] = "CLOSING",
    [TIME_WAIT] = "TIME_WAIT",
    [CLOSE_WAIT] = "CLOSE_WAIT"
};

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

    for(int i = 0; i < size; i++) {
        buffer2[i] = data[i];
    }
    
    pheader->src_ip = socket->local_ip.ip;
    pheader->dst_ip = socket->remote_ip.ip;
    pheader->protocol = 0x0600;
    pheader->total_length = ((total_length & 0x00FF) << 8) | ((total_length & 0xFF00) >> 8);    
    
    header->checksum = 0;
    header->checksum = ipv4_checksum((uint16_t*)packet, length_phdr);
    
    ipv4_send(&socket->stack->tcp->handler, socket->stack, socket->remote_ip, socket->route_mac, (uint8_t*) header, total_length);
	vmm_free(packet, TO_PAGES(length_phdr));
}

void tcp_socket_disconnect(tcp_socket_t* socket, async_t* async) {
    switch (async->state) {
        case STATE_INIT:
            if (socket->state == ESTABLISHED) {
                socket->state = FIN_WAIT1;
                tcp_socket_send_internal(socket, NULL, 0, FIN | ACK);
                socket->sequence_number++;
            }
            async->state = STATE_WAIT;
            break;

        case STATE_WAIT:
            if (socket->state != CLOSED) {
                break;
            }

            for (int i = 0; i < socket->stack->tcp->num_binds; i++) {
                if (socket->stack->tcp->binds[i].socket == socket) {
                    socket->stack->tcp->binds[i].socket = NULL;
                    break;
                }
            }

            vmm_free(socket, PAGES_OF(tcp_socket_t));
            async->state = STATE_DONE;
            break;
    }
}


void tcp_socket_send(tcp_socket_t* socket, uint8_t* data, int size) {
    tcp_socket_send_internal(socket, data, size, PSH | ACK);
}

void tcp_set_local_port(tcp_socket_t* socket, uint16_t port) {
    debugf("Setting local port to %d", port);
    socket->local_port = BSWAP16(port);
}

tcp_socket_t* tcp_connect(network_stack_t* stack, async_t* async, ip_u ip, uint16_t port) {
    if (async->data == NULL) {
        mac_u route = ipv4_resolve_route(stack, async, ip);
        if (!is_resolved(async)) {
            return NULL;
        }

        tcp_socket_t* socket = vmm_alloc(PAGES_OF(tcp_socket_t));
        memset(socket, 0, sizeof(tcp_socket_t));

        socket->remote_ip = ip;
        socket->remote_port = port;
        socket->local_port = stack->tcp->free_port++;
        socket->local_ip = stack->driver->ip_config.ip;
        socket->route_mac = route;

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
                async->data = socket;
                async->state = STATE_WAIT;
                tcp_socket_send_internal(socket, NULL, 0, SYN);
                return NULL;
            }
        }

        stack->tcp->binds = vmm_resize(sizeof(tcp_bind_t), stack->tcp->num_binds, stack->tcp->num_binds + 1, stack->tcp->binds);
        stack->tcp->binds[stack->tcp->num_binds] = bind;
        stack->tcp->num_binds++;

        async->data = socket;
        async->state = STATE_WAIT;
        tcp_socket_send_internal(socket, NULL, 0, SYN);
        return NULL;
    }

    if (async->state == STATE_WAIT) {
        tcp_socket_t* socket = (tcp_socket_t*) async->data;
        if (socket->state == ESTABLISHED) {
            async->state = STATE_DONE;
            return socket;
        }
        return NULL;
    }

    if (async->state == STATE_DONE) {
        return (tcp_socket_t*) async->data;
    }

    async->state = STATE_WAIT;
    return NULL;
}

tcp_socket_t* tcp_listen(network_stack_t* stack, uint16_t port);

void tcp_ipv4_recv(struct ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size) {
    if (size < 20) {
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
        int prev_state = socket->state;
        bool reset = false;

        uint32_t header_bytes = (uint32_t)tcp->header_size_32 * 4;
        if (header_bytes < 20 || header_bytes > size) {
            return;
        }

        uint32_t payload_bytes = size - header_bytes;
        uint8_t* payload_data = payload + header_bytes;
        uint32_t recv_seq = BSWAP32(tcp->sequence_number);
        uint32_t recv_ack = BSWAP32(tcp->ack_number);

        if ((tcp->flags & RST) != 0) {
            socket->state = CLOSED;
        }

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
                        socket->ack_number = recv_seq + 1;
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
				if (recv_seq != socket->ack_number) {
					tcp_socket_send_internal(socket, NULL, 0, ACK);
					break;
				}

				socket->ack_number = recv_seq + payload_bytes + 1;
				if(socket->state == ESTABLISHED) {
					socket->state = CLOSE_WAIT;
					tcp_socket_send_internal(socket, NULL, 0, ACK);
					tcp_socket_send_internal(socket, NULL, 0, FIN | ACK);
					socket->sequence_number++;
				} else if(socket->state == CLOSE_WAIT) {
					socket->state = CLOSED;
				} else if(socket->state == FIN_WAIT1 || socket->state == FIN_WAIT2) {
					socket->state = CLOSED;
					tcp_socket_send_internal(socket, NULL, 0, ACK);
                } else {
                    reset = true;
                }
                break;

                case ACK:
                    if(socket->state == SYN_RECEIVED) {
                        socket->state = ESTABLISHED;
                    } else if(socket->state == FIN_WAIT1) {
                        if (recv_ack == socket->sequence_number) {
                            socket->state = FIN_WAIT2;
                        }
                    } else if(socket->state == CLOSE_WAIT) {
                        socket->state = CLOSED;
                        break;
                    }
                    // Fallthrough since ACK can also carry data

                default:
                    if (payload_bytes == 0) {
                        break;
                    }

                    if(recv_seq == socket->ack_number) {
                        socket->recv(socket, payload_data, (int)payload_bytes);
                        socket->ack_number += payload_bytes;
                        tcp_socket_send_internal(socket, NULL, 0, ACK);
                    } else {
                        debugf("data in wrong order");
                        tcp_socket_send_internal(socket, NULL, 0, ACK);
                    }
            }
        }
        if (prev_state != socket->state) {
            debugf("tcp: state %s -> %s", tcp_socket_state_str[prev_state], tcp_socket_state_str[socket->state]);
        }

        if (prev_state != CLOSED && socket->state == CLOSED) {
            debugf("--- WARNING --- TCP socket transitioned to CLOSED. Ensure socket_disconnect() is called to free resources.");
        }

        if(reset) {
            tcp_socket_send_internal(socket, NULL, 0, RST);
            socket->state = CLOSED;
        }

        if (socket->state == CLOSED) {
            debugf("socket closed");
            for (int i = 0; i < socket->stack->tcp->num_binds; i++) {
                if (socket->stack->tcp->binds[i].socket == socket) {
                    socket->stack->tcp->binds[i].socket = NULL;
                    break;
                }
            }
            return;
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
#endif