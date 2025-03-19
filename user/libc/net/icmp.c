#include <net/icmp.h>
#include <net/ipv4.h>
#include <sys/net.h>

#define TIMEOUT 1000

bool sync_icmp_ping(int nic, ip_u ip) {
    async_t async = { .state = STATE_INIT };

    mac_u route = sync_ipv4_resolve_route(nic, ip);

    int timeout = 0;
    while (!is_resolved(&async)) {
        icmp_ping(nic, &async, ip, route);

        sleep_ms(1);
        if (timeout++ > TIMEOUT) {
            return false;
        }
    }

    return true;
}