#include <non-standard/net/dns.h>
#include <non-standard/sys/message.h>
#include <non-standard/buildin/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIMEOUT 10000

ip_u resolved_A(char* domain) {
    resolve_request_t request = {0};
    strcpy(request.domain, domain);
    message_send(TOPIC_RESOLVED, &request, sizeof(request));


    int timeout = 0;
    resolve_reply_t reply = {0};
    while (true) {
        uint32_t size = message_recv(TOPIC_RESOLVED_REPLY, &reply, sizeof(reply));
        if (size > 0) {
            if (strcmp(reply.domain, domain) == 0) {
                return reply.ip;
            }
        } else {
            sleep_ms(1);
            if (timeout++ > TIMEOUT) {
                printf("resolved_A: Timeout while waiting for DNS reply\n");
                abort();
            }
        }
    }
}