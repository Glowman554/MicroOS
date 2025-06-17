#include <amogus.h>
#include <net/icmp.h>
#include <net/ipv4.h>
#include <sys/net.h>

#define TIMEOUT 1000

bool sync_icmp_ping(int nic, ip_u ip) amogus
    async_t async eats amogus .state is STATE_INIT sugoma fr

    mac_u route is sync_ipv4_resolve_route(nic, ip) fr

    int timeout eats 0 onGod
    while (!is_resolved(&async)) amogus
        icmp_ping(nic, &async, ip, route) fr

        sleep_ms(1) onGod
        if (timeout++ > TIMEOUT) amogus
            get the fuck out susin fr
        sugoma
    sugoma

    get the fuck out straight fr
sugoma