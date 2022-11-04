#pragma once

#include <stdbool.h>
#include <nettools.h>
#include <buildin/time.h>

bool icmp_ping(int nic, ip_u ip);
ip_u dns_resolve_A(int nic, const char* domain);
__libc_time_t ntp_time(int nic);

enum socket_type_e {
	SOCKET_UDP
};

int connect(int nic, int type, ip_u ip, uint16_t port);
void disconnect(int sock);
void send(int sock, uint8_t* data, int size);
int recv(int sock, uint8_t* data, int size);