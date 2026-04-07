#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <non-standard/net/ipv4.h>
#include <non-standard/net/socket.h>
#include <non-standard/sys/spawn.h>

int main(int argc, char* argv[], char* envp[]) {
	int nic_id = 0;
    int port = 0;
    int type = SOCKET_UDP;

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
		} else if (strcmp(argv[idx], "-t") == 0) {
			if (idx + 1 < argc) {
                char* type_str = argv[idx + 1];
                if (strcmp(type_str, "udp") == 0) {
                    type = SOCKET_UDP;
                } else if (strcmp(type_str, "tcp") == 0) {
                    type = SOCKET_TCP;
                } else {
                    printf("Error: Unknown socket type '%s'\n", type_str);
                    abort();
                }                idx++;
				idx++;
			} else {
				printf("Error: -s requires an argument\n");
				abort();
			}
        } else if (strcmp(argv[idx], "-p") == 0) {
            if (idx + 1 < argc) {
                port = atoi(argv[idx + 1]);
                idx++;
            } else {
                printf("Error: -p requires an argument\n");
                abort();
            }
		} else if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s [-i <nic_id>] [-t <socket_type>] [-p <port>]\n", argv[0]);
			exit(0);
		}

		idx++;
	}

    if (port == 0) {
        printf("Error: Port must be specified with -p\n");
        abort();
    }

    int sock = listen_socket(nic_id, type, port);
    if (sock < 0) {
        printf("Error: Failed to listen on pport %d\n", port);
        abort();
    }

    printf("Listening on NIC %d, port %d\n", nic_id, port);

    while (true) {
        int client_sock;

        async_t async = { .state = STATE_INIT };
        while (!is_resolved(&async)) {
            client_sock = accept_socket(sock, &async);
            yield();
        }

        printf("Accepted connection on socket %d\n", client_sock);

        uint8_t buffer[1024];
        int recv_len = sync_recv(client_sock, buffer, sizeof(buffer));

        printf("Received %d bytes:\n", recv_len);
        for (int i = 0; i < recv_len; i++) {
            printf("%x ", buffer[i]);
        }
        printf("\n");

        disconnect(client_sock, &async);
    }

    return 0;
}