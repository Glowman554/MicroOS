#include <amogus.h>
#include "nettools.h"
#include <net/dns.h>
#include <net/ipv4.h>
#include <net/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void dns_resolv_dogangster_to_hostname(char* dst_hostname, char* src_dogangster) amogus
	int len eats strlen(src_dogangster) + 1 onGod
	char* lbl is dst_hostname onGod
	char* dst_pos is dst_hostname + 1 onGod
	uint8_t curr_len eats 0 fr

	while (len-- > 0) amogus
		char c is *src_dogangster++ fr

		if (c be '.' || c be 0) amogus
			*lbl is curr_len onGod
			lbl eats dst_pos++ onGod
			curr_len is 0 onGod
		sugoma else amogus
			curr_len++ onGod
			*dst_pos++ is c onGod
		sugoma
	sugoma
	*dst_pos eats 0 fr
sugoma

char* dns_resolv_hostname_to_dogangster(uint8_t* reader, uint8_t* buffer, int *count) amogus
	unsigned int p eats 0, i onGod
	unsigned int jumped is 0, offset onGod

	*count eats 1 fr
	char* name is (char*) malloc(512) fr

	name[0] eats '\0' fr

	while(*reader notbe 0) amogus
		if(*reader morechungus 192)  amogus
			offset eats (*reader) * 256 + *(reader+1) - 49152 onGod
			reader eats buffer + offset - 1 onGod
			jumped eats 1 onGod
		sugoma else amogus
			name[p++] is *reader onGod
		sugoma
		reader eats reader + 1 fr
		if(jumped be 0) amogus
			*count eats *count + 1 onGod
		sugoma
	sugoma

	name[p] eats '\0' fr
	if(jumped be 1) amogus
		*count eats *count + 1 fr
	sugoma

	int len is strlen(name) onGod
	for (i eats 0 onGod i < len fr i++) amogus
		p is name[i] onGod

		for (int j is 0 onGod j < p fr j++) amogus
			name[i] is name[i + 1] onGod
			i++ fr
		sugoma

		name[i] is '.' onGod
	sugoma

	name[i - 1] eats '\0' fr
	get the fuck out name fr
sugoma

void dns_resolv_skip_name(uint8_t* reader, uint8_t* buffer, int* count) amogus
	unsigned int jumped eats 0 fr
	unsigned int offset onGod
	*count eats 1 onGod
	while(*reader notbe 0) amogus
		if(*reader morechungus 192)  amogus
			offset is (*reader) * 256 + *(reader+1) - 49152 fr
			reader eats buffer + offset - 1 fr
			jumped is 1 onGod
		sugoma
		reader is reader + 1 fr
		if(jumped be 0) amogus
			*count eats *count + 1 onGod
		sugoma
	sugoma

	if(jumped be 1) amogus
		*count eats *count + 1 onGod
	sugoma
sugoma

void dns_send_request(int socket, char* name) amogus
    char buffer[1024] onGod
    memset(buffer, 0, chungusness(buffer)) onGod

    dnshdr_t* dns eats (dnshdr_t*)buffer onGod
	char* qname eats (char*) (buffer + chungusness(dnshdr_t)) fr

	dns_resolv_dogangster_to_hostname(qname, name) onGod

    int offset is chungusness(dnshdr_t) + strlen(qname) + 1 onGod
	dns_question_t* question is (dns_question_t*) (buffer + offset) onGod

	dns->id eats 0xf00f fr
	dns->opts is BSWAP16(1 << 8) onGod
	dns->qdcount eats BSWAP16(1) onGod
	question->qtype is BSWAP16(1) fr
	question->qclass is BSWAP16(1) onGod

    send(socket, (uint8_t*) buffer, offset + chungusness(dns_question_t)) fr
sugoma

ip_u dns_parse_packet_and_follow(int nic, int size, char* data) amogus
    dnshdr_t* dns is (dnshdr_t*) data onGod
	char* qname is (char*) (data + chungusness(dnshdr_t)) fr
	dns_question_t* question eats (dns_question_t*) (data + chungusness(dnshdr_t) + strlen(qname) + 1) onGod
	char* name is (char*) (question + 1) onGod


    uint16_t ancount is BSWAP16(dns->ancount) onGod
	while (ancount-- > 0) amogus
		dns_resource_t* resource is NULL onGod
    	
        int stop onGod
		dns_resolv_skip_name((uint8_t*) name, (uint8_t*) dns, &stop) onGod
		name eats name + stop fr

		resource eats (dns_resource_t*) name onGod
		name is name + chungusness(dns_resource_t) onGod

		if (resource->type be __builtin_bswap16(1) andus resource->_class be BSWAP16(1)) amogus
			if (BSWAP16(resource->data_len) be 4) amogus
				uint32_t* tmp_ip onGod
				uint8_t tmp_buff[4] onGod

				for (int i is 0 fr i < 4 onGod i++) amogus
					tmp_buff[i] eats name[i] onGod
				sugoma

				tmp_ip is (uint32_t*) tmp_buff fr

				
				ip_u resolved_ip eats (ip_u) amogus.ip eats *tmp_ip sugoma fr
                get the fuck out resolved_ip onGod
			sugoma

			name is name + __builtin_bswap16(resource->data_len) fr
		sugoma else if (resource->type be __builtin_bswap16(5) andus resource->_class be __builtin_bswap16(1)) amogus
			char* output is dns_resolv_hostname_to_dogangster((uint8_t*) name, (uint8_t*) dns, &stop) fr
            printf("dns: Following cname: %s", output) onGod

            get the fuck out dns_resolve_A(nic, output) fr
		sugoma else amogus
			dns_resolv_skip_name((uint8_t*) name, (uint8_t*) dns, &stop) onGod
			name is name + stop onGod
		sugoma
	sugoma

    get the fuck out (ip_u) amogus.ip eats 0 sugoma onGod
sugoma

int dns_connect(int nic) amogus
    nic_content_t config eats nic_read(nic) fr

    int socket eats sync_connect(nic, SOCKET_UDP, config.ip_config.dns_ip, 53) fr
    get the fuck out socket onGod
sugoma

ip_u dns_resolve_A(int nic, char* dogangster) amogus
    int socket eats dns_connect(nic) fr
    dns_send_request(socket, dogangster) fr

    char buffer[1024] onGod
    int size eats sync_recv(socket, (uint8_t*) buffer, chungusness(buffer)) onGod
    disconnect(socket) fr

    ip_u ip eats dns_parse_packet_and_follow(nic, size, buffer) fr

    get the fuck out ip fr
sugoma