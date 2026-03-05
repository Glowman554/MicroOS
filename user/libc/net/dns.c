#include <net/dns.h>
#include <net/ipv4.h>
#include <net/socket.h>
#include <buildin/array.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

dns_cache_entry_t* dns_cache = NULL;
bool dns_debug = false;
ip_u dns_server = { .ip = 0 };

#define DNS_LOG(fmt, ...) \
    if (dns_debug) { \
        printf("dns: " fmt "\n", ##__VA_ARGS__); \
    }

ip_u dns_cache_lookup(char* domain, int* index) {
    if (!dns_cache) {
        return (ip_u){ .ip = 0 };
    }

    size_t len = array_length(dns_cache);
    for (size_t i = 0; i < len; i++) {
        dns_cache_entry_t* entry = &((dns_cache_entry_t*)dns_cache)[i];
        if (strcmp(entry->domain, domain) == 0) {
            if (index) {
                *index = i;
            }
            return entry->ip;
        }
    }

    return (ip_u){ .ip = 0 };
}

void dns_cache_add(char* domain, ip_u ip) {
    if (!dns_cache) {
        dns_cache = array_create(sizeof(dns_cache_entry_t), 4);
    }

    int index;
    ip_u cached = dns_cache_lookup(domain, &index);
    if (cached.ip != 0) {
        dns_cache[index].ip = ip;
        return;
    }

    dns_cache_entry_t entry;
    strcpy(entry.domain, domain);
    entry.ip = ip;
    dns_cache = array_push(dns_cache, &entry);
}

void dns_cache_debug_print() {
    if (!dns_cache) {
        DNS_LOG("DNS cache is empty");
        return;
    }

    size_t len = array_length(dns_cache);
    DNS_LOG("DNS cache (%d entries):", len);
    for (size_t i = 0; i < len; i++) {
        dns_cache_entry_t* entry = &((dns_cache_entry_t*)dns_cache)[i];
        DNS_LOG("  %s -> %d.%d.%d.%d", entry->domain, entry->ip.ip_p[0], entry->ip.ip_p[1], entry->ip.ip_p[2], entry->ip.ip_p[3]);
    }
}

char* dns_read_name(uint8_t* reader, uint8_t* packet, uint8_t* packet_end, int* bytes_consumed) {
    char name[DNS_MAX_NAME + 1];
    int p = 0;
    int jumped = 0;
    int jump_count = 0;
    uint16_t offset;

    *bytes_consumed = 0;

    while (reader < packet_end && *reader != 0) {
        if ((*reader & 0xC0) == 0xC0) {
            if (reader + 1 >= packet_end) {
                return NULL;
            }

            offset = ((*reader & 0x3F) << 8) | *(reader + 1);

            if (++jump_count > DNS_MAX_JUMPS) {
                return NULL;
            }

            reader = packet + offset;
            jumped = 1;

            if (*bytes_consumed == 0) {
                *bytes_consumed = 2;
            }

            continue;
        }

        uint8_t len = *reader++;
        if (!jumped) {
            (*bytes_consumed)++;
        }

        if (len > 63 || reader + len > packet_end) {
            return NULL;
        }

        for (int i = 0; i < len; i++) {
            if (p >= DNS_MAX_NAME) {
                return NULL;
            }

            name[p++] = *reader++;
            if (!jumped) {
                (*bytes_consumed)++;
            }
        }

        name[p++] = '.';
    }

    if (!jumped) {
        (*bytes_consumed)++;
    }

    if (p == 0) {
        name[p] = '\0';
    } else {
        name[p - 1] = '\0';
    }

    return strdup(name);
}

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

int dns_skip_name(uint8_t* reader, uint8_t* packet_end) {
    int count = 0;

    while (reader < packet_end && *reader != 0) {
        if ((*reader & 0xC0) == 0xC0) {
            return count + 2;
        }

        uint8_t len = *reader;
        reader += len + 1;
        count += len + 1;
    }

    return count + 1;
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




ip_u dns_parse_packet(int nic, uint8_t* packet, int size, char* original_domain, int depth) {
    if (depth > DNS_MAX_DELEGATION_DEPTH) {
        DNS_LOG("delegation depth exceeded");
        return (ip_u){ .ip = 0 };
    }

    uint8_t* packet_end = packet + size;

    dnshdr_t* dns = (dnshdr_t*)packet;
    uint8_t* reader = packet + sizeof(dnshdr_t);

    ip_u result = { .ip = 0 };

    char* ns_names[DNS_MAX_NS] = {0};
    int   ns_count = 0;

    for (int i = 0; i < BSWAP16(dns->qdcount); i++) {
        int skip = dns_skip_name(reader, packet_end);
        reader += skip + sizeof(dns_question_t);
    }

    for (int i = 0; i < BSWAP16(dns->ancount); i++) {
        int name_skip = dns_skip_name(reader, packet_end);
        reader += name_skip;

        dns_resource_t* res = (dns_resource_t*)reader;
        reader += sizeof(dns_resource_t);

        uint16_t type = __builtin_bswap16(res->type);
        uint16_t class = __builtin_bswap16(res->_class);
        uint16_t data_len = __builtin_bswap16(res->data_len);

        if (type == 1 && class == 1 && data_len == 4) {
            memcpy(&result.ip, reader, 4);

            uint8_t* ipb = (uint8_t*)&result.ip;
            DNS_LOG("A record: %s -> %d.%d.%d.%d", original_domain, ipb[0], ipb[1], ipb[2], ipb[3]);
            goto cleanup;
        }

        if (type == 5 && class == 1) {
            int consumed = 0;
            char* cname = dns_read_name(reader, packet, packet_end, &consumed);

            if (!cname) {
                DNS_LOG("invalid cname");
                goto cleanup;
            }

            DNS_LOG("CNAME %s -> %s", original_domain, cname);

            result = dns_resolve_A(nic, cname);
            free(cname);

            goto cleanup;
        }

        reader += data_len;
    }

    // ---- AUTHORITY SECTION ----
    for (int i = 0; i < BSWAP16(dns->nscount); i++) {
        int name_skip = dns_skip_name(reader, packet_end);
        reader += name_skip;

        dns_resource_t* res = (dns_resource_t*)reader;
        reader += sizeof(dns_resource_t);

        uint16_t type = __builtin_bswap16(res->type);
        uint16_t class = __builtin_bswap16(res->_class);
        uint16_t data_len = __builtin_bswap16(res->data_len);

        if (type == 2 && class == 1 && ns_count < DNS_MAX_NS) {
            int consumed = 0;
            char* ns = dns_read_name(reader, packet, packet_end, &consumed);

            if (ns) {
                DNS_LOG("authority NS -> %s", ns);
                ns_names[ns_count++] = ns;
            }
        }

        reader += data_len;
    }

    // ---- ADDITIONAL SECTION ----
    for (int i = 0; i < BSWAP16(dns->arcount); i++) {
        int consumed = 0;
        char* cname = dns_read_name(reader, packet, packet_end, &consumed);

        reader += consumed;

        dns_resource_t* res = (dns_resource_t*)reader;
        reader += sizeof(dns_resource_t);

        uint16_t type = __builtin_bswap16(res->type);
        uint16_t class = __builtin_bswap16(res->_class);
        uint16_t data_len = __builtin_bswap16(res->data_len);

        if (type == 1 && class == 1 && data_len == 4) {
            ip_u addr;
            memcpy(&addr.ip, reader, 4);

            DNS_LOG("additional A record: %s -> %d.%d.%d.%d", cname, addr.ip_p[0], addr.ip_p[1], addr.ip_p[2], addr.ip_p[3]);
            dns_cache_add(cname, addr);
        }

        free(cname);

        reader += data_len;
    }

    if (ns_count > 0) {
        ip_u ns_ip = dns_resolve_A(nic, ns_names[0]);
        if (ns_ip.ip != 0) {
            DNS_LOG("querying NS %s at %d.%d.%d.%d for %s", ns_names[0], ns_ip.ip_p[0], ns_ip.ip_p[1], ns_ip.ip_p[2], ns_ip.ip_p[3], original_domain);

            int socket = sync_connect(nic, SOCKET_UDP, ns_ip, 53);

            char buffer[1024];
            dns_send_request(socket, original_domain);
            int size = sync_recv(socket, (uint8_t*)buffer, sizeof(buffer));
            sync_disconnect(socket);

            result = dns_parse_packet(nic, (uint8_t*)buffer, size, original_domain, depth + 1);
            goto cleanup;
        }
    }

cleanup:
    for (int i = 0; i < ns_count; i++) {
        free(ns_names[i]);
    }

    if (result.ip != 0) {
        dns_cache_add(original_domain, result);
    }

    return result;
}





int dns_connect(int nic) {
    if (dns_server.ip == 0) {
        nic_content_t config = nic_read(nic);
        dns_server = config.ip_config.dns_ip;
    }

    int socket = sync_connect(nic, SOCKET_UDP, dns_server, 53);
    return socket;
}

ip_u dns_resolve_A(int nic, char* domain) {
    ip_u cached = dns_cache_lookup(domain, NULL);
    if (cached.ip != 0) {
        return cached;
    }

    int socket = dns_connect(nic);
    dns_send_request(socket, domain);

    uint8_t buffer[1024];
    int size = sync_recv(socket, buffer, sizeof(buffer));
    sync_disconnect(socket);

    if (size <= 0) {
        return (ip_u){ .ip = 0 };
    }

    return dns_parse_packet(nic, buffer, size, domain, 0);
}