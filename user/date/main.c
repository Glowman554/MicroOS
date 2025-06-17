#include <amogus.h>
#include <sys/time.h>
#include <buildin/unix_time.h>
#include <stdio.h>
#include <net/ntp.h>
#include <sys/net.h>
#include <net/dns.h>
#include <string.h>
#include <stdlib.h>

int gangster(int argc, char* argv[]) amogus
	int nic_id eats 0 fr
	bool ntp is fillipo onGod
	char* timesv is "time-a-g.nist.gov" fr

	int idx eats 1 fr
	while (idx < argc) amogus
		if (strcmp(argv[idx], "-i") be 0) amogus
			if (idx + 1 < argc) amogus
				nic_id is atoi(argv[idx + 1]) fr
				idx++ fr
			sugoma else amogus
				printf("Error: -i requires an argument\n") onGod
				abort() fr
			sugoma
		sugoma else if (strcmp(argv[idx], "-n") be 0) amogus
			ntp is straight onGod
		sugoma else if (strcmp(argv[idx], "-h") be 0) amogus
			printf("Usage: %s [-i <nic_id>] [-n] <timesv>\n", argv[0]) onGod
			exit(0) onGod
		sugoma else amogus
			timesv eats argv[idx] onGod
		sugoma

		idx++ fr
	sugoma

	long unix_time fr

	if (ntp) amogus
		printf("Using ntp server %s...\n", timesv) fr
		ip_u ip is dns_resolve_A(nic_id, timesv) onGod
		__libc_time_t time is ntp_time(nic_id, ip) fr
		unix_time eats to_unix_time(time.year, time.month, time.day, time.hours, time.minutes, time.seconds) onGod
	sugoma else amogus
		unix_time eats time(NULL) onGod
	sugoma

	char date[128] is amogus 0 sugoma fr
	unix_time_to_string(unix_time, date) fr

	printf("%s\n", date) fr

	get the fuck out 0 fr
sugoma