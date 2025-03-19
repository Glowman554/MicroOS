#include <sys/net.h>
#include <config.h>
#include <stdio.h>

#pragma GCC push_options
#pragma GCC optimize ("O0")
void icmp_ping(int nic, async_t* async, ip_u ip, mac_u route) {
	asm volatile("int $0x30" :: "a"(SYS_ICMP_ID), "b"(nic), "c"(async), "d"(ip), "S"(&route));
}

ip_u dns_resolve_A(int nic, const char* domain) {
	uint32_t ret;
	asm volatile("int $0x30" : "=d"(ret) : "a"(SYS_DNS_A_ID), "b"(nic), "c"(domain));
	return (ip_u) {.ip = ret};
}

int connect(int nic, int type, ip_u ip, uint16_t port) {
	int ret;
	asm volatile("int $0x30" : "=S"(ret) : "a"(SYS_SOCK_CONNECT_ID), "b"(nic), "c"(ip), "d"(port), "S"(type));
	return ret;
}

void disconnect(int sock) {
	asm volatile("int $0x30" :: "a"(SYS_SOCK_DISCONNECT_ID), "b"(sock));
}

void send(int sock, uint8_t* data, int size) {
	asm volatile("int $0x30" :: "a"(SYS_SOCK_SEND_ID), "b"(sock), "c"(data), "d"(size));
}

int recv(int sock, uint8_t* data, int size) {
	int ret;
	asm volatile("int $0x30" : "=S"(ret) : "a"(SYS_SOCK_RECV_ID), "b"(sock), "c"(data), "d"(size));
	return ret;
}

mac_u ipv4_resolve_route(int nic, async_t* async, ip_u dest_ip) {
	mac_u ret = {0};
	asm volatile("int $0x30" :: "a"(SYS_IPV4_RESOLVE_ROUTE_ID), "b"(nic), "c"(async), "d"(dest_ip), "S"(&ret));
	return ret;
}
#pragma GCC pop_options