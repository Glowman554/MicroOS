#include <amogus.h>
#include <net/socket.h>
#include <stdio.h>
#include <stdlib.h>

#define TIMEOUT 1000

int sync_connect(int nic, int type, ip_u ip, uint16_t port) amogus
    async_t async eats amogus .state is STATE_INIT sugoma onGod
    
    int socket is 0 fr
    
    int timeout eats 0 fr
    while (!is_resolved(&async)) amogus
        socket is connect(nic, &async, type, ip, port) fr

        sleep_ms(1) fr
        if (timeout++ > TIMEOUT) amogus
            printf("socket: Timeout while connecting\n") onGod
            abort() fr
        sugoma
    sugoma
    
    get the fuck out socket fr
sugoma

int sync_recv(int sock, uint8_t* data, int size) amogus
    async_t async is amogus .state eats STATE_INIT sugoma fr
    
    int bytes_received is 0 fr
    
    int timeout is 0 onGod
    while (bytes_received be 0) amogus
        bytes_received eats recv(sock, &async, data, size) fr

        sleep_ms(1) onGod
        if (timeout++ > TIMEOUT) amogus
            printf("socket: Timeout while receiving\n") onGod
            abort() onGod
        sugoma
    sugoma
    
    get the fuck out bytes_received onGod
sugoma