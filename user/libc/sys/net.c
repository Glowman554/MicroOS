#include <sys/net.h>
#include <config.h>

bool icmp_ping(int nic, ip_u ip) {
	bool ret;
	asm volatile("int $0x30" : "=d"(ret) : "a"(SYS_ICMP_ID), "b"(nic), "c"(ip));
	return ret;
}

ip_u dns_resolve_A(int nic, const char* domain) {
	uint32_t ret;
	asm volatile("int $0x30" : "=d"(ret) : "a"(SYS_DNS_A_ID), "b"(nic), "c"(domain));
	return (ip_u) {.ip = ret};
}

__libc_time_t ntp_time(int nic) {
	__libc_time_t time = { 0 };
	asm volatile("int $0x30" :: "a"(SYS_NTP_ID), "b"(nic), "c"(&time));
	return time;
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
	asm volatile("int $0x30" : "=S"(ret) : "a"(SYS_SOCK_SEND_ID), "b"(sock), "c"(data), "d"(size));
	return ret;
}