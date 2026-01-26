#include <net/tcp.h>

#include <memory/heap.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <driver/timer_driver.h>
#include <scheduler/async.h>
#include <config.h>
#include <utils/random.h>
#ifdef NETWORK_STACK
#ifdef TCP

uint32_t tcp_now_ms(void) {
    return global_timer_driver->time_ms(global_timer_driver);
}

bool tcp_seq_after_eq(uint32_t a, uint32_t b) {
    return (int32_t)(a - b) >= 0;
}

void tcp_tx_clear_unacked(tcp_socket_t* socket) {
    if (socket->tx_unacked_data != NULL) {
        kfree(socket->tx_unacked_data);
        socket->tx_unacked_data = NULL;
    }
    socket->tx_unacked_len = 0;
    socket->tx_unacked_seq = 0;
    socket->tx_unacked_flags = 0;
    socket->tx_last_send_ms = 0;
    socket->tx_retries = 0;
}

void tcp_tx_track_unacked(tcp_socket_t* socket, const uint8_t* data, uint32_t len, uint16_t flags, uint32_t seq_at_send) {
    if (len == 0 || (flags & PSH) == 0) {
        return;
    }

    tcp_tx_clear_unacked(socket);

    socket->tx_unacked_data = kmalloc(len);
    memcpy(socket->tx_unacked_data, data, len);
    socket->tx_unacked_len = len;
    socket->tx_unacked_seq = seq_at_send;
    socket->tx_unacked_flags = flags;
    socket->tx_last_send_ms = tcp_now_ms();
    socket->tx_retries = 0;
    if (socket->tx_max_retries == 0) {
        socket->tx_max_retries = TCP_RETRANSMIT_MAX_RETRIES;
    }
}

void tcp_socket_resend_unacked(tcp_socket_t* socket) {
    if (socket->tx_unacked_data == NULL || socket->tx_unacked_len == 0) {
        return;
    }

    uint16_t total_length = (uint16_t)(socket->tx_unacked_len + sizeof(tcp_header_t));
    uint16_t length_phdr = (uint16_t)(total_length + sizeof(tcp_pseudo_header_t));

    uint8_t* packet = (uint8_t*) kmalloc(length_phdr);

    tcp_pseudo_header_t* pheader = (tcp_pseudo_header_t*) packet;
    tcp_header_t* header = (tcp_header_t*) (packet + sizeof(tcp_pseudo_header_t));
    uint8_t* buffer2 = packet + sizeof(tcp_header_t) + sizeof(tcp_pseudo_header_t);

    header->header_size_32 = sizeof(tcp_header_t) / 4;
    header->src_port = socket->local_port;
    header->dst_port = socket->remote_port;

    header->ack_number = BSWAP32(socket->ack_number);
    header->sequence_number = BSWAP32(socket->tx_unacked_seq);
    header->reserved = 0;
    header->flags = (uint8_t)socket->tx_unacked_flags;
    header->window_size = 0xFFFF;
    header->urgent_ptr = 0;
    header->options = 0;

    memcpy(buffer2, socket->tx_unacked_data, socket->tx_unacked_len);

    pheader->src_ip = socket->local_ip.ip;
    pheader->dst_ip = socket->remote_ip.ip;
    pheader->protocol = 0x0600;
    pheader->total_length = ((total_length & 0x00FF) << 8) | ((total_length & 0xFF00) >> 8);

    header->checksum = 0;
    header->checksum = ipv4_checksum((uint16_t*)packet, length_phdr);

#if TCP_SIMULATED_DROP_PERCENT > 0
    if (rng_chance(TCP_SIMULATED_DROP_PERCENT)) {
        debugf("tcp: dropped outgoing packet (retransmit), %d bytes, flags=0x%x, drop=%d%%", socket->tx_unacked_len, socket->tx_unacked_flags, TCP_SIMULATED_DROP_PERCENT);
    } else {
#endif
        ipv4_send(&socket->stack->tcp->handler, socket->stack, socket->remote_ip, socket->route_mac, (uint8_t*) header, total_length);
#if TCP_SIMULATED_DROP_PERCENT > 0
    }
#endif

    kfree(packet);

    socket->tx_last_send_ms = tcp_now_ms();
    socket->tx_retries++;
}

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
    uint32_t seq_consume = ((flags & (SYN | FIN)) != 0) ? 1u : 0u;

    uint32_t seq_at_send = socket->sequence_number;

    uint16_t total_length = (uint16_t)(size + (int)sizeof(tcp_header_t));
    uint16_t length_phdr = (uint16_t)(total_length + sizeof(tcp_pseudo_header_t));

    uint8_t* packet = (uint8_t*) kmalloc(length_phdr);

    tcp_pseudo_header_t* pheader = (tcp_pseudo_header_t*) packet;
    tcp_header_t* header = (tcp_header_t*) (packet + sizeof(tcp_pseudo_header_t));
    uint8_t* buffer2 = packet + sizeof(tcp_header_t) + sizeof(tcp_pseudo_header_t);

    header->header_size_32 = sizeof(tcp_header_t) / 4;
    header->src_port = socket->local_port;
    header->dst_port = socket->remote_port;

    header->ack_number = BSWAP32(socket->ack_number);
    header->sequence_number = BSWAP32(socket->sequence_number);
    header->reserved = 0;
    header->flags = (uint8_t)flags;
    header->window_size = 0xFFFF;
    header->urgent_ptr = 0;

    header->options = ((flags & SYN) != 0) ? 0xB4050402 : 0;

    if (data != NULL && size > 0) {
        memcpy(buffer2, data, (size_t)size);
    } else {
        size = 0;
    }

    if (data != NULL && size > 0) {
        tcp_tx_track_unacked(socket, data, (uint32_t)size, flags, seq_at_send);
    }

    pheader->src_ip = socket->local_ip.ip;
    pheader->dst_ip = socket->remote_ip.ip;
    pheader->protocol = 0x0600;
    pheader->total_length = ((total_length & 0x00FF) << 8) | ((total_length & 0xFF00) >> 8);

    header->checksum = 0;
    header->checksum = ipv4_checksum((uint16_t*)packet, length_phdr);

#if TCP_SIMULATED_DROP_PERCENT > 0
    if (rng_chance(TCP_SIMULATED_DROP_PERCENT)) {
        debugf("tcp: dropped outgoing packet, %d bytes, flags=0x%x, drop=%d%%", size, flags, TCP_SIMULATED_DROP_PERCENT);
    } else {
#endif
        ipv4_send(&socket->stack->tcp->handler, socket->stack, socket->remote_ip, socket->route_mac, (uint8_t*) header, total_length);
#if TCP_SIMULATED_DROP_PERCENT > 0
    }
#endif

    kfree(packet);

    socket->sequence_number += (uint32_t)size + seq_consume;
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

            tcp_tx_clear_unacked(socket);
            kfree(socket);
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

        tcp_socket_t* socket = kmalloc(sizeof(tcp_socket_t));
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

        stack->tcp->binds = kmalloc(sizeof(tcp_bind_t) * (stack->tcp->num_binds + 1));
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
    tcp_header_t* tcp = (tcp_header_t*) payload;

    uint32_t header_bytes = (uint32_t)tcp->header_size_32 * 4;
    if (header_bytes < 20 || header_bytes > size) {
        return;
    }

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
        return;
    }

    int prev_state = socket->state;
    bool reset = false;

    uint32_t payload_bytes = size - header_bytes;
    uint8_t* payload_data = payload + header_bytes;
    uint32_t recv_seq = BSWAP32(tcp->sequence_number);
    uint32_t recv_ack = BSWAP32(tcp->ack_number);

    if ((tcp->flags & ACK) != 0 && socket->tx_unacked_len != 0) {
        uint32_t end_seq = socket->tx_unacked_seq + socket->tx_unacked_len;
        if (!tcp_seq_after_eq(recv_ack, socket->tx_unacked_seq) || tcp_seq_after_eq(recv_ack, socket->sequence_number + 1)) {
        } else if (tcp_seq_after_eq(recv_ack, end_seq)) {
            tcp_tx_clear_unacked(socket);
        }
    }

    if ((tcp->flags & RST) != 0) {
        socket->state = CLOSED;
    }

    if (socket->state != CLOSED) {
        switch((tcp->flags) & (SYN | ACK | FIN)) {
            case SYN:
                reset = true;
                break;

            case SYN | ACK:
                if(socket->state == SYN_SENT) {
                    socket->state = ESTABLISHED;
                    socket->ack_number = recv_seq + 1;
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
                    tcp_socket_send_internal(socket, NULL, 0, ACK);
                }
        }
    }

    if (prev_state != socket->state) {
        debugf("tcp: state %s -> %s", tcp_socket_state_str[prev_state], tcp_socket_state_str[socket->state]);
    }

    if(reset) {
        tcp_socket_send_internal(socket, NULL, 0, RST);
        socket->state = CLOSED;
    }

    if (socket->state == CLOSED) {
        tcp_tx_clear_unacked(socket);
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

void tcp_poll(network_stack_t* stack) {
    uint32_t now = tcp_now_ms();

    for (int i = 0; i < stack->tcp->num_binds; i++) {
        tcp_socket_t* s = stack->tcp->binds[i].socket;
        if (s == NULL) {
            continue;
        }

        if (s->state == CLOSED) {
            continue;
        }

        if (s->state == SYN_SENT) {
            if (s->tx_last_send_ms == 0) {
                s->tx_last_send_ms = now;
            }
            if ((now - s->tx_last_send_ms) >= TCP_CTL_TIMEOUT_MS) {
                if (s->tx_retries >= TCP_CTL_MAX_RETRIES) {
                    debugf("tcp: SYN retry limit reached, closing socket");
                    s->state = CLOSED;
                    tcp_tx_clear_unacked(s);
                    continue;
                }
                s->tx_last_send_ms = now;
                s->tx_retries++;
                tcp_socket_send_internal(s, NULL, 0, SYN);
            }
            continue;
        }

        if (s->tx_unacked_len == 0 || s->tx_unacked_data == NULL) {
            continue;
        }

        if (s->tx_last_send_ms == 0) {
            s->tx_last_send_ms = now;
            continue;
        }

        uint32_t timeout = TCP_RETRANSMIT_TIMEOUT_MS;
        if (s->tx_retries > 0) {
            uint32_t mul = 1u << (s->tx_retries > 4 ? 4 : s->tx_retries);
            timeout *= mul;
        }

        if ((now - s->tx_last_send_ms) >= timeout) {
            uint8_t maxr = (s->tx_max_retries ? s->tx_max_retries : TCP_RETRANSMIT_MAX_RETRIES);
            if (s->tx_retries >= maxr) {
                debugf("tcp: retransmit limit reached, closing socket");
                s->state = CLOSED;
                tcp_tx_clear_unacked(s);
                continue;
            }

            debugf("tcp: retransmitting %d bytes (try %d)", (unsigned)s->tx_unacked_len, (unsigned)(s->tx_retries + 1));
            tcp_socket_resend_unacked(s);
        }
    }
}

void tcp_poll_task(async_t* async) {
    network_stack_t* stack = (network_stack_t*) async->data;
    tcp_poll(stack);
}

void tcp_init(network_stack_t* stack) {
	stack->tcp = kmalloc(sizeof(tcp_provider_t));
	memset(stack->tcp, 0, sizeof(tcp_provider_t));

	stack->tcp->free_port = 1024;

	stack->tcp->handler.protocol = 0x06;
	stack->tcp->handler.stack = stack;
	stack->tcp->handler.recv = tcp_ipv4_recv;
	ipv4_register(stack, stack->tcp->handler);

    async_t async = {
        .state = STATE_INIT,
        .data = stack
    };
    add_async_task(tcp_poll_task, async, false);
}
#endif
#endif