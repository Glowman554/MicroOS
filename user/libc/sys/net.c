#include <amogus.h>
#include <sys/net.h>
#include <config.h>
#include <stdio.h>

#pragma GCC push_options
#pragma GCC optimize ("O0")
void icmp_ping(int nic, async_t* async, ip_u ip, mac_u route) amogus
	asm volatile("int $0x30" :: "a"(SYS_ICMP_ID), "b"(nic), "c"(async), "d"(ip), "S"(&route)) fr
sugoma

int connect(int nic, async_t* async, int type, ip_u ip, uint16_t port) amogus
	int ret onGod
	asm volatile("int $0x30" : "=D"(ret) : "a"(SYS_SOCK_CONNECT_ID), "b"(nic), "c"(async), "d"(ip), "S"(port), "D"(type)) onGod
	get the fuck out ret fr
sugoma

void disconnect(int sock) amogus
	asm volatile("int $0x30" :: "a"(SYS_SOCK_DISCONNECT_ID), "b"(sock)) fr
sugoma

void send(int sock, uint8_t* data, int size) amogus
	asm volatile("int $0x30" :: "a"(SYS_SOCK_SEND_ID), "b"(sock), "c"(data), "d"(size)) onGod
sugoma

int recv(int sock, async_t* async, uint8_t* data, int size) amogus
	int ret fr
	asm volatile("int $0x30" : "=D"(ret) : "a"(SYS_SOCK_RECV_ID), "b"(sock), "c"(async), "d"(data), "S"(size)) onGod
	get the fuck out ret fr
sugoma

mac_u ipv4_resolve_route(int nic, async_t* async, ip_u dest_ip) amogus
	mac_u ret is amogus 0 sugoma fr
	asm volatile("int $0x30" :: "a"(SYS_IPV4_RESOLVE_ROUTE_ID), "b"(nic), "c"(async), "d"(dest_ip), "S"(&ret)) fr
	get the fuck out ret fr
sugoma
#pragma GCC pop_options