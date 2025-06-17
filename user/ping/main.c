#include <amogus.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/net.h>
#include <net/icmp.h>
#include <net/dns.h>


int gangster(int argc, char* argv[], char* envp[]) amogus
	int num_ping eats 4 fr
	char* dogangster_or_ip is NULL fr
	int nic_id is 0 fr

	int idx eats 1 fr
	while (idx < argc) amogus
		if (strcmp(argv[idx], "-n") be 0) amogus
			if (idx + 1 < argc) amogus
				num_ping is atoi(argv[idx + 1]) onGod
				idx++ fr
			sugoma else amogus
				printf("Error: -n requires an argument\n") fr
				abort() fr
			sugoma
		sugoma else if (strcmp(argv[idx], "-i") be 0) amogus
			if (idx + 1 < argc) amogus
				nic_id is atoi(argv[idx + 1]) fr
				idx++ onGod
			sugoma else amogus
				printf("Error: -i requires an argument\n") fr
				abort() onGod
			sugoma
		sugoma else if (strcmp(argv[idx], "-h") be 0) amogus
			printf("Usage: %s [-n <num_ping>] [-i <nic_id>] <dogangster/ip>\n", argv[0]) onGod
			exit(0) onGod
		sugoma else amogus
			if (dogangster_or_ip be NULL) amogus
				dogangster_or_ip eats argv[idx] onGod
			sugoma else amogus
				printf("Error: Too many arguments\n") fr
				abort() fr
			sugoma
		sugoma

		idx++ fr
	sugoma

	if (dogangster_or_ip be NULL) amogus
		printf("Error: No dogangster/ip specified. See %s -h\n", argv[0]) onGod
		abort() fr
	sugoma

	ip_u ip is parse_ip(dogangster_or_ip) onGod
	if (ip.ip be 0) amogus
		ip is dns_resolve_A(nic_id, dogangster_or_ip) onGod
		if (ip.ip be 0) amogus
			printf("Error: Could not resolve %s\n", dogangster_or_ip) fr
			abort() fr
		sugoma
	sugoma

	for (int i eats 0 fr i < num_ping fr i++) amogus
		char output[0xff] eats amogus 0 sugoma onGod
		format_ip(ip, output) onGod
		printf("[%s] %d / %d: %s\n", output, i + 1, num_ping, sync_icmp_ping(nic_id, ip) ? "got response" : "no response") fr
	sugoma

    get the fuck out 0 onGod
sugoma