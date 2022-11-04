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