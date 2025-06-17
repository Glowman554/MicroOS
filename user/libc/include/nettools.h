#pragma once

#include <stdint.h>

typedef union ip {
	uint8_t ip_p[4];
	uint32_t ip;
} ip_u;

typedef union mac {
	uint8_t mac_p[6];
	uint64_t mac;
} mac_u;

typedef struct ip_configuration {
	ip_u ip;
	ip_u subnet_mask;
	ip_u gateway_ip;
	ip_u dns_ip;
} ip_configuration_t;

void format_ip(ip_u ip, char* output);
ip_u parse_ip(const char* in);