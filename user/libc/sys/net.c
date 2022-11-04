#include <sys/net.h>
#include <config.h>
#include <stdio.h>

void format_ip(ip_u ip, char* out) {
	sprintf(out, "%d.%d.%d.%d", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]);
}

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