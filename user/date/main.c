#include <sys/time.h>
#include <buildin/unix_time.h>
#include <stdio.h>
#include <net/ntp.h>
#include <sys/net.h>
#include <net/dns.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	int nic_id = 0;
	bool ntp = false;
	char* timesv = "time-a-g.nist.gov";

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
		} else if (strcmp(argv[idx], "-n") == 0) {
			ntp = true;
		} else if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s [-i <nic_id>] [-n] <timesv>\n", argv[0]);
			exit(0);
		} else {
			timesv = argv[idx];
		}

		idx++;
	}

	long unix_time;

	if (ntp) {
		printf("Using ntp server %s...\n", timesv);
		ip_u ip = dns_resolve_A(nic_id, timesv);
		__libc_time_t time = ntp_time(nic_id, ip);
		unix_time = to_unix_time(time.year, time.month, time.day, time.hours, time.minutes, time.seconds);
	} else {
		unix_time = time(NULL);
	}

	char date[128] = { 0 };
	unix_time_to_string(unix_time, date);

	printf("%s\n", date);

	return 0;
}