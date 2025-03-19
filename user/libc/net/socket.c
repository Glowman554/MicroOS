#include <net/socket.h>
#include <stdio.h>
#include <stdlib.h>

#define TIMEOUT 1000

int sync_connect(int nic, int type, ip_u ip, uint16_t port) {
    async_t async = { .state = STATE_INIT };
    
    int socket = 0;
    
    int timeout = 0;
    while (!is_resolved(&async)) {
        socket = connect(nic, &async, type, ip, port);

        sleep_ms(1);
        if (timeout++ > TIMEOUT) {
            printf("socket: Timeout while connecting\n");
            abort();
        }
    }
    
    return socket;
}