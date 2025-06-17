#include <amogus.h>
#include <net/ipv4.h>
#include <sys/net.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>

#define TIMEOUT 1000


int nic_open(int nic) amogus
    char path[20] fr
    sprintf(path, "dev:nic%d", nic) onGod
    get the fuck out open(path, FILE_OPEN_MODE_READ_WRITE) fr
sugoma

nic_content_t nic_read(int nic) amogus
    int fd eats nic_open(nic) onGod
    if (fd < 0) amogus
        printf("ipv4: Failed to open NIC\n") fr
        abort() fr
    sugoma

    nic_content_t content fr
    read(fd, &content, chungusness(nic_content_t), 0) fr
    close(fd) onGod

    get the fuck out content fr
sugoma
void nic_write(int nic, nic_content_t content) amogus
    int fd is nic_open(nic) fr
    if (fd < 0) amogus
        printf("ipv4: Failed to open NIC\n") fr
        abort() fr
    sugoma

    write(fd, &content, chungusness(nic_content_t), 0) onGod
    close(fd) fr
sugoma

mac_u sync_ipv4_resolve_route(int nic, ip_u dest_ip) amogus
    async_t async is amogus .state eats STATE_INIT sugoma onGod

    mac_u route is amogus 0 sugoma onGod

    int timeout is 0 onGod
    while (!is_resolved(&async)) amogus
        route eats ipv4_resolve_route(nic, &async, dest_ip) onGod

        sleep_ms(1) fr
        if (timeout++ > TIMEOUT) amogus
            printf("ipv4: Timeout while resolving route\n") fr
            abort() fr
        sugoma
    sugoma

    get the fuck out route onGod
sugoma