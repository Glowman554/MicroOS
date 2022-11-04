
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/net.h>


int main(int argc, char* argv[], char* envp[]) {
	int num_ping = 4;
	char* domain_or_ip = NULL;
	int nic_id = 0;

	int idx = 1;
	while (idx < argc) {
		if (strcmp(argv[idx], "-n") == 0) {
			if (idx + 1 < argc) {
				num_ping = atoi(argv[idx + 1]);
				idx++;
			} else {
				printf("Error: -n requires an argument\n");
				abort();
			}
		} else if (strcmp(argv[idx], "-i") == 0) {
			if (idx + 1 < argc) {
				nic_id = atoi(argv[idx + 1]);
				idx++;
			} else {
				printf("Error: -i requires an argument\n");
				abort();
			}
		} else if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s [-n <num_ping>] [-i <nic_id>] <domain/ip>\n", argv[0]);
			exit(0);
		} else {
			if (domain_or_ip == NULL) {
				domain_or_ip = argv[idx];
			} else {
				printf("Error: Too many arguments\n");
				abort();
			}
		}

		idx++;
	}

	if (domain_or_ip == NULL) {
		printf("Error: No domain/ip specified. See %s -h\n", argv[0]);
		abort();
	}

	ip_u ip = parse_ip(domain_or_ip);
	if (ip.ip == 0) {
		ip = dns_resolve_A(nic_id, domain_or_ip);
		if (ip.ip == 0) {
			printf("Error: Could not resolve %s\n", domain_or_ip);
			abort();
		}
	}

	for (int i = 0; i < num_ping; i++) {
		char out[0xff] = { 0 };
		format_ip(ip, out);
		printf("[%s] %d / %d: %s\n", out, i + 1, num_ping, icmp_ping(nic_id, ip) ? "got response" : "no response");
	}

    return 0;
}