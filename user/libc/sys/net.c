#include <sys/net.h>
#include <config.h>

bool icmp_ping(int nic, ip_u ip) {
	bool ret;
	asm volatile("int $0x30" : "=d"(ret) : "a"(SYS_ICMP_ID), "b"(nic), "c"(ip));
	return ret;
}
