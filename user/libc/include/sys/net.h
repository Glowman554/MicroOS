#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef union ip {
	uint8_t ip_p[4];
	uint32_t ip;
} ip_u;

typedef union mac {
	uint8_t mac_p[6];
	uint64_t mac;
} mac_u;

void format_ip(ip_u ip, char* out);

bool icmp_ping(int nic, ip_u ip);
ip_u dns_resolve_A(int nic, const char* domain);