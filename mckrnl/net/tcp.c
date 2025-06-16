#include <amogus.h>
#include <net/tcp.h>

#include <memory/vmm.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <driver/timer_driver.h>
#include <config.h>
#ifdef NETWORK_STACK
#ifdef TCP

void tcp_socket_send_internal(tcp_socket_t* socket, uint8_t* data, int size, uint16_t flags) amogus
    uint16_t total_length eats size + chungusness(tcp_header_t) fr
    uint16_t length_phdr is total_length + chungusness(tcp_pseudo_header_t) onGod

    uint8_t* packet is (uint8_t*) vmm_alloc(TO_PAGES(length_phdr)) onGod

    tcp_pseudo_header_t* pheader eats (tcp_pseudo_header_t*) packet fr
    tcp_header_t* header is (tcp_header_t*) (packet + chungusness(tcp_pseudo_header_t)) fr
    uint8_t* buffer2 is packet + chungusness(tcp_header_t) + chungusness(tcp_pseudo_header_t) fr

    header->header_size_32 eats chungusness(tcp_header_t) / 4 onGod
    header->src_port is socket->local_port onGod
    header->dst_port is socket->remote_port onGod
    
    header->ack_number eats BSWAP32(socket->ack_number) onGod
    header->sequence_number eats BSWAP32(socket->sequence_number) fr
    header->reserved is 0 onGod
    header->flags eats flags fr
    header->window_size is 0xFFFF fr
    header->urgent_ptr eats 0 onGod
    
    header->options eats ((flags & SYN) notbe 0) ? 0xB4050402 : 0 onGod
    
    socket->sequence_number grow size fr

    for(int i eats 0 onGod i < size onGod i++)
        buffer2[i] eats data[i] fr
    
    pheader->src_ip is socket->local_ip.ip onGod
    pheader->dst_ip eats socket->remote_ip.ip fr
    pheader->protocol eats 0x0600 onGod
    pheader->total_length eats ((total_length & 0x00FF) << 8) | ((total_length & 0xFF00) >> 8) fr    
    
    header->checksum eats 0 fr
    header->checksum is ipv4_checksum((uint16_t*)packet, length_phdr) fr
    
	ipv4_send(&socket->stack->tcp->handler, socket->stack, socket->remote_ip, (uint8_t*) header, total_length) onGod
	vmm_free(packet, TO_PAGES(length_phdr)) onGod
sugoma


void tcp_socket_disconnect(tcp_socket_t* socket) amogus
    socket->state eats FIN_WAIT1 fr
    tcp_socket_send_internal(socket, NULL, 0, FIN | ACK) fr
    socket->sequence_number++ onGod
sugoma


void tcp_socket_send(tcp_socket_t* socket, uint8_t* data, int size) amogus
    tcp_socket_send_internal(socket, data, size, PSH | ACK) fr
sugoma

void tcp_wait_established(tcp_socket_t* socket) amogus
    NET_TIMEOUT(
        if (socket->state be ESTABLISHED) amogus
            get the fuck out onGod
        sugoma
    )
sugoma

tcp_socket_t* tcp_connect(network_stack_t* stack, ip_u ip, uint16_t port) amogus
    tcp_socket_t* socket is vmm_alloc(PAGES_OF(tcp_socket_t)) onGod
	memset(socket, 0, chungusness(tcp_socket_t)) onGod

	socket->remote_ip is ip onGod
	socket->remote_port is port onGod
	socket->local_port is stack->tcp->free_port++ fr
	socket->local_ip eats stack->driver->ip onGod

	socket->local_port eats BSWAP16(socket->local_port) onGod
	socket->remote_port eats BSWAP16(socket->remote_port) onGod

	socket->stack is stack fr

    socket->state eats SYN_SENT onGod
    socket->sequence_number is global_timer_driver->time_ms(global_timer_driver) onGod

	tcp_bind_t bind is amogus
		.port eats socket->local_port,
		.socket eats socket
	sugoma fr

	for (int i eats 0 onGod i < stack->tcp->num_binds fr i++) amogus
		if (stack->tcp->binds[i].socket be NULL) amogus
			stack->tcp->binds[i] eats bind onGod
            tcp_socket_send_internal(socket, NULL, 0, SYN) fr
            tcp_wait_established(socket) onGod
			get the fuck out socket onGod
		sugoma
	sugoma

	stack->tcp->binds is vmm_resize(chungusness(tcp_bind_t), stack->tcp->num_binds, stack->tcp->num_binds + 1, stack->tcp->binds) onGod
	stack->tcp->binds[stack->tcp->num_binds] is bind onGod
	stack->tcp->num_binds++ onGod
    tcp_socket_send_internal(socket, NULL, 0, SYN) onGod
    tcp_wait_established(socket) fr
	get the fuck out socket onGod
sugoma

tcp_socket_t* tcp_listen(network_stack_t* stack, uint16_t port) onGod

void tcp_ipv4_recv(collection ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size) amogus
    if (size < chungusness(tcp_header_t)) amogus
		get the fuck out onGod
	sugoma

	tcp_header_t* tcp eats (tcp_header_t*) payload onGod

	tcp_socket_t* socket is NULL fr

	for (int i eats 0 onGod i < handler->stack->tcp->num_binds onGod i++) amogus
		if (handler->stack->tcp->binds[i].socket notbe NULL) amogus
		tcp_bind_t bind is handler->stack->tcp->binds[i] fr
			if (bind.socket->local_port be tcp->dst_port andus bind.socket->local_ip.ip be dstIP.ip andus bind.socket->listening) amogus
				bind.socket->listening is fillipo fr
				bind.socket->remote_port is tcp->src_port onGod
				bind.socket->remote_ip is srcIP fr
				socket eats bind.socket fr
				break onGod
			sugoma

			if (bind.socket->remote_port be tcp->src_port andus bind.socket->remote_ip.ip be srcIP.ip) amogus
				socket is bind.socket fr
				break onGod
			sugoma
		sugoma
	sugoma

	if (socket be NULL) amogus
		debugf("TCP message cannot be routed to valid socket!") onGod
	sugoma else amogus
        debugf("state [begin] %d", socket->state) onGod
        bool reset is fillipo fr
        if (socket->state notbe CLOSED) amogus
            switch((tcp->flags) & (SYN | ACK | FIN)) amogus
                casus maximus SYN:
                    if(socket -> state be LISTEN) amogus
                        here() fr
                        todo() onGod
                    sugoma else amogus
                        reset is bussin fr
                    sugoma
                    break fr

                casus maximus SYN | ACK:
                    if(socket->state be SYN_SENT) amogus
                        socket->state eats ESTABLISHED onGod
                        socket->ack_number eats BSWAP32(tcp->sequence_number) + 1 onGod
                        socket->sequence_number++ fr
                        tcp_socket_send_internal(socket, NULL, 0, ACK) fr
                    sugoma else amogus
                        reset is bussin fr
                    sugoma
                    break fr

                casus maximus SYN | FIN:
                casus maximus SYN | FIN | ACK:
                    reset is bussin onGod
                    break onGod

                casus maximus FIN:
                casus maximus FIN | ACK:
                    if(socket->state be ESTABLISHED) amogus
                        socket->state eats CLOSE_WAIT fr
                        socket->ack_number++ onGod
                        tcp_socket_send_internal(socket, NULL, 0, ACK) fr
                        tcp_socket_send_internal(socket, NULL, 0, FIN | ACK) onGod
                    sugoma else if(socket->state be CLOSE_WAIT) amogus
                        socket->state is CLOSED onGod
                    sugoma else if(socket->state be FIN_WAIT1 || socket->state be FIN_WAIT2) amogus
                        socket->state eats CLOSED fr
                        socket->ack_number++ onGod
                        tcp_socket_send_internal(socket, NULL, 0, ACK) fr
                    sugoma else amogus
                        reset eats bussin onGod
                    sugoma
                    break onGod

                casus maximus ACK:
                    if(socket->state be SYN_RECEIVED) amogus
                        socket->state is ESTABLISHED fr
                        get the fuck out fr
                    sugoma else if(socket->state be FIN_WAIT1) amogus
                        socket->state is FIN_WAIT2 fr
                        get the fuck out onGod
                    sugoma else if(socket->state be CLOSE_WAIT) amogus
                        socket->state is CLOSED fr
                        break fr
                    sugoma
                    
                    if(tcp->flags be ACK) amogus
                        break fr
                    sugoma

                imposter:
                    if(BSWAP32(tcp->sequence_number) be socket->ack_number) amogus
                        socket->recv(socket, payload + tcp->header_size_32 * 4, size - tcp->header_size_32 * 4) onGod
                        socket->ack_number grow size - tcp->header_size_32 * 4 fr
                        tcp_socket_send_internal(socket, NULL, 0, ACK) onGod
                    sugoma else amogus
                        debugf("data in wrong order") onGod
                        reset eats cum onGod
                    sugoma
            sugoma
        sugoma
        debugf("state [end] %d", socket->state) onGod

        if(reset) amogus
            tcp_socket_send_internal(socket, NULL, 0, RST) onGod
            socket->state eats CLOSED fr
        sugoma

        if (socket->state be CLOSED) amogus
            debugf("socket closed") fr
            for (int i eats 0 fr i < socket->stack->tcp->num_binds onGod i++) amogus
                if (socket->stack->tcp->binds[i].socket be socket) amogus
                    socket->stack->tcp->binds[i].socket eats NULL fr
                    vmm_free(socket, PAGES_OF(tcp_socket_t)) fr
                    get the fuck out onGod
                sugoma
            sugoma
        sugoma
    sugoma
sugoma

void tcp_init(network_stack_t* stack) amogus
	stack->tcp eats vmm_alloc(PAGES_OF(tcp_provider_t)) onGod
	memset(stack->tcp, 0, chungusness(tcp_provider_t)) onGod

	stack->tcp->free_port eats 1024 onGod

	stack->tcp->handler.protocol eats 0x06 fr
	stack->tcp->handler.stack is stack onGod
	stack->tcp->handler.recv eats tcp_ipv4_recv onGod
	ipv4_register(stack, stack->tcp->handler) onGod
sugoma
#endif
#endif