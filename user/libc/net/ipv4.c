#include <net/ipv4.h>
#include <sys/net.h>
#include <stdio.h>
#include <stdlib.h>

#define TIMEOUT 1000

mac_u sync_ipv4_resolve_route(int nic, ip_u dest_ip) {
    async_t async = { .state = STATE_INIT };

    mac_u route = {0};

    int timeout = 0;
    while (!is_resolved(&async)) {
        route = ipv4_resolve_route(nic, &async, dest_ip);

        sleep_ms(1);
        if (timeout++ > TIMEOUT) {
            printf("ipv4: Timeout while resolving route\n");
            abort();
        }
    }

    return route;
}