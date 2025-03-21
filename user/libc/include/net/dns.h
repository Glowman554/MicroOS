#pragma once

#include <stdint.h>
#include <nettools.h>

typedef struct dnshdr {
	uint16_t id;
	uint16_t opts;
	uint16_t qdcount;
	uint16_t ancount;
	uint16_t nscount;
	uint16_t arcount;
} dnshdr_t;

typedef struct dns_question {
	uint16_t qtype;
	uint16_t qclass;
} dns_question_t;

typedef struct dns_resource {
	uint16_t type;
	uint16_t _class;
	uint32_t ttl;
	uint16_t data_len;
} __attribute__((packed)) dns_resource_t;

ip_u dns_resolve_A(int nic, char* domain);