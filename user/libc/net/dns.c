#include "nettools.h"
#include <net/dns.h>
#include <net/ipv4.h>
#include <net/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
	char* name = (char*) malloc(512);

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

void dns_send_request(int socket, char* name) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    dnshdr_t* dns = (dnshdr_t*)buffer;
	char* qname = (char*) (buffer + sizeof(dnshdr_t));

	dns_resolv_domain_to_hostname(qname, name);

    int offset = sizeof(dnshdr_t) + strlen(qname) + 1;
	dns_question_t* question = (dns_question_t*) (buffer + offset);

	dns->id = 0xf00f;
	dns->opts = BSWAP16(1 << 8);
	dns->qdcount = BSWAP16(1);
	question->qtype = BSWAP16(1);
	question->qclass = BSWAP16(1);

    send(socket, (uint8_t*) buffer, offset + sizeof(dns_question_t));
}

ip_u dns_parse_packet_and_follow(int nic, int size, char* data) {
    dnshdr_t* dns = (dnshdr_t*) data;
	char* qname = (char*) (data + sizeof(dnshdr_t));
	dns_question_t* question = (dns_question_t*) (data + sizeof(dnshdr_t) + strlen(qname) + 1);
	char* name = (char*) (question + 1);


    uint16_t ancount = BSWAP16(dns->ancount);
	while (ancount-- > 0) {
		dns_resource_t* resource = NULL;
    	
        int stop;
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
                return resolved_ip;
			}

			name = name + __builtin_bswap16(resource->data_len);
		} else if (resource->type == __builtin_bswap16(5) && resource->_class == __builtin_bswap16(1)) {
			char* out = dns_resolv_hostname_to_domain((uint8_t*) name, (uint8_t*) dns, &stop);
            printf("dns: Following cname: %s", out);

            return dns_resolve_A(nic, out);
		} else {
			dns_resolv_skip_name((uint8_t*) name, (uint8_t*) dns, &stop);
			name = name + stop;
		}
	}

    return (ip_u) {.ip = 0};
}

int dns_connect(int nic) {
    nic_content_t config = nic_read(nic);

    int socket = sync_connect(nic, SOCKET_UDP, config.ip_config.dns_ip, 53);
    return socket;
}

ip_u dns_resolve_A(int nic, char* domain) {
    int socket = dns_connect(nic);
    dns_send_request(socket, domain);

    char buffer[1024];
    int size = sync_recv(socket, (uint8_t*) buffer, sizeof(buffer));
    disconnect(socket);

    ip_u ip = dns_parse_packet_and_follow(nic, size, buffer);

    return ip;
}