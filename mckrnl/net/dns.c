#include <net/dns.h>
#include <memory/vmm.h>
#include <string.h>
#include <stddef.h>
#include <config.h>
#ifdef NETWORK_STACK

void dns_resolv_domain_to_hostname(char* dst_hostname, char* src_domain) {
	int len = strlen(src_domain) + 1;
	char* lbl = dst_hostname;
	char* dst_pos = dst_hostname + 1;
	uint8_t curr_len = 0;

	while (len-- > 0) {
		char c = *src_domain++;

		if (c == '.' || c == 0) {
			*lbl = curr_len;
			lbl = dst_pos++;
			curr_len = 0;
		} else {
			curr_len++;
			*dst_pos++ = c;
		}
	}
	*dst_pos = 0;
}

char* dns_resolv_hostname_to_domain(uint8_t* reader, uint8_t* buffer, int *count) {
	unsigned int p = 0, i;
	unsigned int jumped = 0, offset;

	*count = 1;
	char* name = (char*) vmm_alloc(1);

	name[0] = '\0';

	while(*reader != 0) {
		if(*reader >= 192)  {
			offset = (*reader) * 256 + *(reader+1) - 49152;
			reader = buffer + offset - 1;
			jumped = 1;
		} else {
			name[p++] = *reader;
		}
		reader = reader + 1;
		if(jumped == 0) {
			*count = *count + 1;
		}
	}

	name[p] = '\0';
	if(jumped == 1) {
		*count = *count + 1;
	}

	int len = strlen(name);
	for (i = 0; i < len; i++) {
		p = name[i];

		for (int j = 0; j < p; j++) {
			name[i] = name[i + 1];
			i++;
		}

		name[i] = '.';
	}

	name[i - 1] = '\0';
	return name;
}

void dns_resolv_skip_name(uint8_t* reader, uint8_t* buffer, int* count) {
	unsigned int jumped = 0;
	unsigned int offset;
	*count = 1;
	while(*reader != 0) {
		if(*reader >= 192)  {
			offset = (*reader) * 256 + *(reader+1) - 49152;
			reader = buffer + offset - 1;
			jumped = 1;
		}
		reader = reader + 1;
		if(jumped == 0) {
			*count = *count + 1;
		}
	}

	if(jumped == 1) {
		*count = *count + 1;
	}
}

dns_result_t* dns_find_result(network_stack_t* stack, char* qname) {
	for (int i = 0; i < stack->dns->num_results; i++) {
		if (strcmp(qname, stack->dns->results[i].name) == 0) {
			return &stack->dns->results[i];
		}
	}

	return NULL;
}

#define MAX_LOOP_COUNT 10

ip_u dns_resolve_A(network_stack_t* stack, char* domain) {
	char qname[128];
	memset(qname, 0, 128);
	dns_resolv_domain_to_hostname(qname, domain);

	dns_result_t* _result = dns_find_result(stack, qname);
	if (_result) {
		dns_result_t result = *_result; // copy since the pointer might get invalid when calling dns_resolve_A
		if (result.ipv4.ip != 0) {
			stack->dns->loop = 0;
			return result.ipv4;
		} else {
			if (stack->dns->loop >= MAX_LOOP_COUNT) {
				debugf("DNS: CACHE: looping CNAME: \"%s\"", result.cname);
				return (ip_u) {.ip = 0};
			} else {
				debugf("DNS: CACHE: Resolving \"%s\", following CNAME: \"%s\"", domain, result.cname);
				return dns_resolve_A(stack, result.cname);
			}
		}
	}

	stack->dns->wait_for_response = true;
	dns_request(stack, domain);

	NET_TIMEOUT(
		if (!stack->dns->wait_for_response) {
			dns_result_t* _result = dns_find_result(stack, qname);
			if (_result) {
				dns_result_t result = *_result; // copy since the pointer might get invalid when calling dns_resolve_A
				if (result.ipv4.ip != 0) {
					stack->dns->loop = 0;
					return result.ipv4;
				} else {
					if (stack->dns->loop >= MAX_LOOP_COUNT) {
						debugf("DNS: looping CNAME: \"%s\"", result.cname);
						return (ip_u) {.ip = 0};
					} else {
						debugf("DNS: Resolving \"%s\", following CNAME: \"%s\"", domain, result.cname);
						return dns_resolve_A(stack, result.cname);
					}
				}
			}
		}
	);
	return (ip_u) {.ip = 0};
}

void dns_request(network_stack_t* stack, char* name) {
	char* buffer = (char*) vmm_alloc(1);
	memset(buffer, 0, 4096);
	dnshdr_t* dns = (dnshdr_t*)buffer;
	char* qname = (char*) (buffer + sizeof(dnshdr_t));

	dns_resolv_domain_to_hostname(qname, name);

	dns_question_t* question = (dns_question_t*) (buffer + sizeof(dnshdr_t) + strlen(qname) + 1);

	dns->id = 0xf00f;
	dns->opts = BSWAP16(1 << 8);
	dns->qdcount = BSWAP16(1);
	question->qtype = BSWAP16(1);
	question->qclass = BSWAP16(1);

	udp_socket_send(stack->dns->socket, (uint8_t*) buffer, sizeof(dnshdr_t) + strlen(qname) + 1 + sizeof(dns_question_t));

	vmm_free(buffer, 1);
}

void dns_udp_recv(struct udp_socket* socket, uint8_t* data, int size) {
dnshdr_t* dns = (dnshdr_t*) data;
	char* qname = (char*) (data + sizeof(dnshdr_t));
	dns_question_t* question = (dns_question_t*) (data + sizeof(dnshdr_t) + strlen(qname) + 1);
	char* name = (char*) (question + 1);

	int stop;
	uint16_t ancount;

	dns_result_t result;
	memset(&result, 0, sizeof(dns_result_t));
	strcpy(result.name, qname);
	bool did_get_result = false;

	if (dns->id != 0xf00f) {
		socket->stack->dns->wait_for_response = false;
	}

	ancount = BSWAP16(dns->ancount);
	while (ancount-- > 0) {
		dns_resource_t* resource = NULL;
		dns_resolv_skip_name((uint8_t*) name, (uint8_t*) dns, &stop);
		name = name + stop;

		resource = (dns_resource_t*) name;
		name = name + sizeof(dns_resource_t);

		if (resource->type == __builtin_bswap16(1) && resource->_class == BSWAP16(1)) {
			if (BSWAP16(resource->data_len) == 4) {
				uint32_t* tmp_ip;
				uint8_t tmp_buff[4];

				for (int i = 0; i < 4; i++) {
					tmp_buff[i] = name[i];
				}

				tmp_ip = (uint32_t*) tmp_buff;

				
				ip_u resolved_ip = (ip_u) {.ip = *tmp_ip};

				debugf("DNS: Response \"%s\" -> %d.%d.%d.%d", qname, resolved_ip.ip_p[0], resolved_ip.ip_p[1], resolved_ip.ip_p[2], resolved_ip.ip_p[3]);

				result.ipv4 = resolved_ip;
				did_get_result = true;
			}

			name = name + __builtin_bswap16(resource->data_len);
		} else if (resource->type == __builtin_bswap16(5) && resource->_class == __builtin_bswap16(1)) {
			char* out = dns_resolv_hostname_to_domain((uint8_t*) name, (uint8_t*) dns, &stop);
			strcpy(result.cname, out);

			debugf("DNS: Response \"%s\" -> \"%s\"\n", qname, result.cname);

			did_get_result = true;

			name = name + stop;
		} else {
			dns_resolv_skip_name((uint8_t*) name, (uint8_t*) dns, &stop);
			name = name + stop;
		}
	}

	if (did_get_result) {
		socket->stack->dns->loop += 1;
		
		socket->stack->dns->results = vmm_resize(sizeof(dns_result_t), socket->stack->dns->num_results, socket->stack->dns->num_results + 1, socket->stack->dns->results);
		socket->stack->dns->results[socket->stack->dns->num_results] = result;
		socket->stack->dns->num_results++;
	}

	socket->stack->dns->wait_for_response = false;
	return;
}

void dns_init(network_stack_t* stack, async_t* async, ip_u dns_server) {
	udp_socket_t* socket = udp_connect(stack, async, dns_server, 53);
	if (is_resolved(async)) {
		stack->dns = vmm_alloc(PAGES_OF(dns_provider_t));
		memset(stack->dns, 0, sizeof(dns_provider_t));

		stack->dns->socket = socket;
		stack->dns->socket->recv = dns_udp_recv;
	}
}
#endif