#pragma once

#include <net/stack.h>
#include <net/udp.h>
#include <utils/time.h>

typedef struct ntp_packet {
	uint8_t li_vn_mode;
	uint8_t stratum;
	uint8_t poll;
	uint8_t precision;
	uint32_t rootDelay;
	uint32_t rootDispersion;
	uint32_t refId;
	uint32_t refTm_s;
	uint32_t refTm_f;
	uint32_t origTm_s;
	uint32_t origTm_f;
	uint32_t rxTm_s;
	uint32_t rxTm_f;
	uint32_t txTm_s;
	uint32_t txTm_f;
} __attribute__((packed)) ntp_packet_t;

typedef struct ntp_provider {
    udp_socket_t* socket;
    ntp_packet_t last_packet;
    bool received_packet;
} ntp_provider_t;

void ntp_udp_recv(struct udp_socket* socket, uint8_t* data, int size);

time_t ntp_time(network_stack_t* stack);

void ntp_init(network_stack_t* stack, ip_u ntp_server);
