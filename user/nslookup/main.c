#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/net.h>

int main(int argc, char* argv[], char* envp[]) {
	char* domain = NULL;
	int nic_id = 0;

	int idx = 1;
	while (idx < argc) {
		if (strcmp(argv[idx], "-i") == 0) {
			if (idx + 1 < argc) {
				nic_id = atoi(argv[idx + 1]);
				idx++;
			} else {
				printf("Error: -i requires an argument\n");
				abort();
			}
		} else if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s [-i <nic_id>] <domain>\n", argv[0]);
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

	ip_u ip = dns_resolve_A(nic_id, domain);
	if (ip.ip == 0) {
		printf("Error: Could not resolve %s\n", domain);
		abort();
	} else {
		printf("%s: %d.%d.%d.%d\n", domain, ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]);
	}

    return 0;
}