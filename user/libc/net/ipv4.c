#include <net/ipv4.h>
#include <sys/net.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>

#define TIMEOUT 1000


int nic_open(int nic) {
    char path[20];
    sprintf(path, "dev:nic%d", nic);
    return open(path, FILE_OPEN_MODE_READ_WRITE);
}

nic_content_t nic_read(int nic) {
    int fd = nic_open(nic);
    if (fd < 0) {
        printf("ipv4: Failed to open NIC\n");
        abort();
    }

    nic_content_t content;
    read(fd, &content, sizeof(nic_content_t), 0);
    close(fd);

    return content;
}
void nic_write(int nic, nic_content_t content) {
    int fd = nic_open(nic);
    if (fd < 0) {
        printf("ipv4: Failed to open NIC\n");
        abort();
    }

    write(fd, &content, sizeof(nic_content_t), 0);
    close(fd);
}

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