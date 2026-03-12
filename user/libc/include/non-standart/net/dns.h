#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <non-standart/net/nettools.h>

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

#define DNS_MAX_NAME               255
#define DNS_MAX_JUMPS              10
#define DNS_MAX_CNAME_DEPTH        5
#define DNS_MAX_DELEGATION_DEPTH   5
#define DNS_MAX_NS                 4

typedef struct dns_cache_entry {
	char domain[DNS_MAX_NAME + 1];
	ip_u ip;
} dns_cache_entry_t;

ip_u dns_resolve_A(int nic, char* domain);
void dns_cache_debug_print();

extern bool dns_debug;
extern ip_u dns_server;