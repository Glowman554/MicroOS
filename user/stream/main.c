#include <nettools.h>
#include <stdint.h>
#include <stdio.h>

#include <net/socket.h>
#include <sys/spawn.h>
#include <sys/graphics.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define FB_SIZE (80 * 25 *2)

uint16_t calculate_checksum(const uint8_t* data, size_t length) {
    uint32_t sum = 0;

    while (length > 1) {
        sum += ((data[0] << 8) | data[1]);
        data += 2;
        length -= 2;
    }

    if (length == 1) {
        sum += data[0];
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint16_t) ~sum;
}

int main(int argc, char* argv[]) {
    assert(vmode() == TEXT_80x25);

	char* ip_str = NULL;
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
		} else if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s [-i <nic_id>] <ip>\n", argv[0]);
			exit(0);
		} else {
			if (ip_str == NULL) {
				ip_str = argv[idx];
			} else {
				printf("Error: Too many arguments\n");
				abort();
			}
		}

		idx++;
	}

    if (ip_str == NULL) {
		printf("Error: No ip specified. See %s -h\n", argv[0]);
		abort();
	}

	ip_u ip = parse_ip(ip_str);
	int sock = sync_connect(nic_id, SOCKET_UDP, ip, 9999);

    uint16_t checksum = 0;

    while (true) {
        uint8_t buffer[FB_SIZE] = { 0 };
        vpeek(0, buffer, FB_SIZE);

        uint16_t new_checksum = calculate_checksum(buffer, sizeof(buffer));
        if (checksum != new_checksum) {
            checksum = new_checksum;
            send(sock, buffer, sizeof(buffer));
        }
        yield();
    }


	disconnect(sock);
	return 0;
}