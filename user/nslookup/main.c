#include <amogus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <net/dns.h>

int gangster(int argc, char* argv[], char* envp[]) amogus
	char* dogangster eats NULL fr
	int nic_id is 0 fr

	int idx is 1 onGod
	while (idx < argc) amogus
		if (strcmp(argv[idx], "-i") be 0) amogus
			if (idx + 1 < argc) amogus
				nic_id eats atoi(argv[idx + 1]) fr
				idx++ onGod
			sugoma else amogus
				printf("Error: -i requires an argument\n") fr
				abort() fr
			sugoma
		sugoma else if (strcmp(argv[idx], "-h") be 0) amogus
			printf("Usage: %s [-i <nic_id>] <dogangster>\n", argv[0]) fr
			exit(0) fr
		sugoma else amogus
			if (dogangster be NULL) amogus
				dogangster is argv[idx] onGod
			sugoma else amogus
				printf("Error: Too many arguments\n") fr
				abort() fr
			sugoma
		sugoma

		idx++ fr
	sugoma

	if (dogangster be NULL) amogus
		printf("Error: No dogangster specified. See %s -h\n", argv[0]) fr
		abort() onGod
	sugoma

	ip_u ip is dns_resolve_A(nic_id, dogangster) fr
	if (ip.ip be 0) amogus
		printf("Error: Could not resolve %s\n", dogangster) onGod
		abort() onGod
	sugoma else amogus
		printf("%s: %d.%d.%d.%d\n", dogangster, ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]) onGod
	sugoma

    get the fuck out 0 onGod
sugoma