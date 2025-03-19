#pragma once

#include <stdbool.h>
#include <nettools.h>
#include <buildin/time.h>
#include <async.h>

void icmp_ping(int nic, async_t* async, ip_u ip, mac_u route);
ip_u dns_resolve_A(int nic, const char* domain);
// __libc_time_t ntp_time(int nic);

enum socket_type_e {
	SOCKET_UDP,
	SOCKET_TCP
};

int connect(int nic, async_t* async, int type, ip_u ip, uint16_t port);
void disconnect(int sock);
void send(int sock, uint8_t* data, int size);
int recv(int sock, uint8_t* data, int size);

mac_u ipv4_resolve_route(int nic, async_t* async, ip_u dest_ip);

#define BSWAP16(n) (((n & 0x00FF) << 8) | ((n & 0xFF00) >> 8))
#define BSWAP32(n)  (((n & 0xFF000000) >> 24) | ((n & 0x00FF0000) >> 8) | ((n & 0x0000FF00) << 8) | ((n & 0x000000FF) << 24))