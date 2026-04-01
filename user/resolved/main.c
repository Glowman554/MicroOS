#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <non-standard/sys/message.h>
#include <non-standard/sys/net.h>
#include <non-standard/net/dns.h>
#include <dns.h>


#include <non-standard/sys/env.h>
#include <non-standard/sys/spawn.h>


int main(int argc, char* argv[]) {
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
		} else if (strcmp(argv[idx], "-s") == 0) {
			if (idx + 1 < argc) {
				dns_server = parse_ip(argv[idx + 1]);
				idx++;
			} else {
				printf("Error: -s requires an argument\n");
				abort();
			}
		} else if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s [-i <nic_id>] [-s <dns_server>] [-v]\n", argv[0]);
			exit(0);
		} else if (strcmp(argv[idx], "-v") == 0) {
			dns_debug = true;
		} else {
			printf("Error: Too many arguments\n");
			abort();
		}

		idx++;
	}

    printf("resolved: Starting on nic %d\n", nic_id);

    resolve_request_t request = {0};
    while (true) {
        uint32_t size = message_recv(TOPIC_RESOLVED, &request, sizeof(request));
        if (size > 0) {
            printf("resolved: Received resolve request for %s\n", request.domain);
            ip_u resolved_ip = dns_resolve_A(0, request.domain);
            printf("resolved: Resolved %s to %d.%d.%d.%d\n", request.domain, resolved_ip.ip_p[0], resolved_ip.ip_p[1], resolved_ip.ip_p[2], resolved_ip.ip_p[3]);

            resolve_reply_t reply = {0};
            strcpy(reply.domain, request.domain);
            reply.ip = resolved_ip;
            message_send(TOPIC_RESOLVED_REPLY, &reply, sizeof(reply));
        } else {
            sleep_ms(100);
        }
    }
}