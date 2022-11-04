#include <sys/net.h>
#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {
	int nic_id = 0;
	ip_u ip = dns_resolve_A(nic_id, argv[1]);
	
	char out[0xff] = { 0 };
	format_ip(ip, out);
	printf("ip (%s): %s\n", argv[1], out);

	printf("ping answerd: %s", icmp_ping(nic_id, ip) ? "true" : "false");
}