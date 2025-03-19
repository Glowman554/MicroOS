#pragma once

#include <net/stack.h>
#include <net/udp.h>
#include <async.h>

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

typedef struct dns_result {
	ip_u ipv4;
	char cname[256];
	char name[256];
} dns_result_t;

typedef struct dns_provider {
	udp_socket_t* socket;

	dns_result_t* results;
	int num_results;

	int loop;
	bool wait_for_response;
} dns_provider_t;

void dns_resolv_domain_to_hostname(char* dst_hostname, char* src_domain);
char* dns_resolv_hostname_to_domain(uint8_t* reader, uint8_t* buffer, int *count);
void dns_resolv_skip_name(uint8_t* reader, uint8_t* buffer, int* count);

ip_u dns_resolve_A(network_stack_t* stack, char* domain);
void dns_request(network_stack_t* stack, char* name);

void dns_udp_recv(struct udp_socket* socket, uint8_t* data, int size);

void dns_init(network_stack_t* stack, async_t* async, ip_u dns_server);