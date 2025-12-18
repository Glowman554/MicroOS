#pragma once

#include <net/stack.h>
#include <net/ipv4.h>
#include <async.h>
#include <stdbool.h>
#include <stdint.h>

enum tcp_socket_state {
    CLOSED,
    LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
            
    ESTABLISHED,
            
    FIN_WAIT1,
    FIN_WAIT2,
    CLOSING,
    TIME_WAIT,
            
    CLOSE_WAIT
};
        
enum tcp_flags {
    FIN = 1,
    SYN = 2,
    RST = 4,
    PSH = 8,
    ACK = 16,
    URG = 32,
    ECE = 64,
    CWR = 128,
    NS = 256
};
        
        
typedef struct tcp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t sequence_number;
    uint32_t ack_number;
            
    uint8_t reserved : 4;
    uint8_t header_size_32 : 4;
    uint8_t flags;
            
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_ptr;
            
    uint32_t options;
} __attribute__((packed)) tcp_header_t;
       
      
typedef struct tcp_pseudo_header {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t protocol;
    uint16_t total_length;
} __attribute__((packed)) tcp_pseudo_header_t;

typedef struct tcp_socket {
	uint16_t remote_port;
	uint16_t local_port;
	ip_u remote_ip;
	ip_u local_ip;
    mac_u route_mac;
	bool listening;

    uint32_t sequence_number;
    uint32_t ack_number;
    int state;

	network_stack_t* stack;

	void* data;

	void (*recv)(struct tcp_socket* socket, uint8_t* data, int size);

    uint8_t* tx_unacked_data;
    uint32_t tx_unacked_len;
    uint32_t tx_unacked_seq;
    uint16_t tx_unacked_flags;
    uint32_t tx_last_send_ms;
    uint8_t  tx_retries;
    uint8_t  tx_max_retries;
} tcp_socket_t;

typedef struct tcp_bind {
	uint16_t port;
	tcp_socket_t* socket;
} tcp_bind_t;

typedef struct tcp_provider {
	tcp_bind_t* binds;
	int num_binds;
	int free_port;
	ipv4_handler_t handler;
} tcp_provider_t;

void tcp_socket_disconnect(tcp_socket_t* socket, async_t* async);
void tcp_socket_send(tcp_socket_t* socket, uint8_t* data, int size);

void tcp_set_local_port(tcp_socket_t* socket, uint16_t port);

tcp_socket_t* tcp_connect(network_stack_t* stack, async_t* async, ip_u ip, uint16_t port);
tcp_socket_t* tcp_listen(network_stack_t* stack, uint16_t port);

void tcp_ipv4_recv(struct ipv4_handler* handler, ip_u srcIP, ip_u dstIP, uint8_t* payload, uint32_t size);

void tcp_poll(network_stack_t* stack);

void tcp_init(network_stack_t* stack);
