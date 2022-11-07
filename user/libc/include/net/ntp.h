#pragma once

#include <sys/net.h>
#include <buildin/time.h>


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

__libc_time_t ntp_time(int nic, ip_u sv);
