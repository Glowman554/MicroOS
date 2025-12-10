#include <sys/net.h>
#include <config.h>
#include <stdio.h>

#pragma GCC push_options
#pragma GCC optimize ("O0")
void icmp_ping(int nic, async_t* async, ip_u ip, mac_u route) {
	asm volatile("int $0x30" :: "a"(SYS_ICMP_ID), "b"(nic), "c"(async), "d"(ip), "S"(&route));
}

int connect(int nic, async_t* async, int type, ip_u ip, uint16_t port) {
	int ret;
	asm volatile("int $0x30" : "=D"(ret) : "a"(SYS_SOCK_CONNECT_ID), "b"(nic), "c"(async), "d"(ip), "S"(port), "D"(type));
	return ret;
}

void disconnect(int sock) {
	asm volatile("int $0x30" :: "a"(SYS_SOCK_DISCONNECT_ID), "b"(sock));
}

void send(int sock, uint8_t* data, int size) {
	asm volatile("int $0x30" :: "a"(SYS_SOCK_SEND_ID), "b"(sock), "c"(data), "d"(size));
}

int recv(int sock, async_t* async, uint8_t* data, int size) {
	int ret;
	asm volatile("int $0x30" : "=D"(ret) : "a"(SYS_SOCK_RECV_ID), "b"(sock), "c"(async), "d"(data), "S"(size));
	return ret;
}

mac_u ipv4_resolve_route(int nic, async_t* async, ip_u dest_ip) {
	mac_u ret = {0};
	asm volatile("int $0x30" :: "a"(SYS_IPV4_RESOLVE_ROUTE_ID), "b"(nic), "c"(async), "d"(dest_ip), "S"(&ret));
	return ret;
}

void set_local_port(int sock, uint16_t port) {
	asm volatile("int $0x30" :: "a"(SYS_SOCK_SET_LOCAL_PORT_ID), "b"(sock), "c"(port));
}

#pragma GCC pop_options