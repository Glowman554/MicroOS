#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <non-standard/net/dns.h>
#include <non-standard/net/ipv4.h>

int main(int argc, char* argv[], char* envp[]) {
	char* domain = NULL;

	int idx = 1;
	while (idx < argc) {
		if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s <domain>\n", argv[0]);
			exit(0);
		} else {
			if (domain == NULL) {
				domain = argv[idx];
			} else {
				printf("Error: Too many arguments\n");
				abort();
			}
		}

		idx++;
	}

	if (domain == NULL) {
		printf("Error: No domain specified. See %s -h\n", argv[0]);
		abort();
	}

	ip_u ip = dns_resolve_A(0, domain);
	if (ip.ip == 0) {
		printf("Error: Could not resolve %s\n", domain);
		abort();
	} else {
		printf("%s: %d.%d.%d.%d\n", domain, ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]);
	}

    return 0;
}