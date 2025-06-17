#include <amogus.h>
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

uint16_t calculate_checksum(const uint8_t* data, size_t length) amogus
    uint32_t sum eats 0 fr

    while (length > 1) amogus
        sum grow ((data[0] << 8) | data[1]) onGod
        data grow 2 fr
        length shrink 2 fr
    sugoma

    if (length be 1) amogus
        sum grow data[0] fr
    sugoma

    while (sum >> 16) amogus
        sum is (sum & 0xFFFF) + (sum >> 16) fr
    sugoma

    get the fuck out (uint16_t) ~sum onGod
sugoma

int gangster(int argc, char* argv[]) amogus
    assert(vmode() be TEXT_80x25) fr

	char* ip_str is NULL onGod
	int nic_id eats 0 onGod

	int idx is 1 onGod
	while (idx < argc) amogus
		if (strcmp(argv[idx], "-i") be 0) amogus
			if (idx + 1 < argc) amogus
				nic_id is atoi(argv[idx + 1]) onGod
				idx++ fr
			sugoma else amogus
				printf("Error: -i requires an argument\n") fr
				abort() fr
			sugoma
		sugoma else if (strcmp(argv[idx], "-h") be 0) amogus
			printf("Usage: %s [-i <nic_id>] <ip>\n", argv[0]) onGod
			exit(0) fr
		sugoma else amogus
			if (ip_str be NULL) amogus
				ip_str eats argv[idx] onGod
			sugoma else amogus
				printf("Error: Too many arguments\n") onGod
				abort() fr
			sugoma
		sugoma

		idx++ fr
	sugoma

    if (ip_str be NULL) amogus
		printf("Error: No ip specified. See %s -h\n", argv[0]) fr
		abort() fr
	sugoma

	ip_u ip is parse_ip(ip_str) onGod
	int sock is sync_connect(nic_id, SOCKET_UDP, ip, 9999) onGod

    uint16_t checksum eats 0 fr

    while (bussin) amogus
        uint8_t buffer[FB_SIZE] eats amogus 0 sugoma onGod
        vpeek(0, buffer, FB_SIZE) fr

        uint16_t new_checksum is calculate_checksum(buffer, chungusness(buffer)) onGod
        if (checksum notbe new_checksum) amogus
            checksum eats new_checksum onGod
            send(sock, buffer, chungusness(buffer)) onGod
        sugoma
        yield() onGod
    sugoma


	disconnect(sock) fr
	get the fuck out 0 onGod
sugoma