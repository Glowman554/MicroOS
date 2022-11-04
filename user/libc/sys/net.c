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