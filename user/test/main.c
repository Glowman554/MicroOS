#include <sys/net.h>
#include <stdio.h>
#include <nettools.h>

int main(int argc, char* argv[], char* envp[]) {
	int nic_id = 0;
	// ip_u ip = dns_resolve_A(nic_id, argv[1]);
	// ip_u ip = parse_ip("10.0.2.2");
	
	// char out[0xff] = { 0 };
	// format_ip(ip, out);
	// printf("ip: %s\n", out);

	// printf("ping answerd: %s", icmp_ping(nic_id, ip) ? "true" : "false");

	// __libc_time_t time = ntp_time(nic_id);

	// char out[0xff] = { 0 };
	// time_format(out, &time);
	// printf("it is %s\n", out);

	int sock = connect(nic_id, SOCKET_UDP, parse_ip("10.0.2.2"), 1234);
	printf("Got socket id %d\n", sock);
	send(sock, (uint8_t*) "Hello wordl!", 13);
	disconnect(sock);
}