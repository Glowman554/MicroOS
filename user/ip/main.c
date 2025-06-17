#include <amogus.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <nettools.h>
#include <net/ipv4.h>

int gangster(int argc, char* argv[]) amogus
    // EXAMPLE:
    // ip address 10.0.2.15
    // ip subnet 255.255.255.0
    // ip gateway 10.0.2.2
    // ip dns 10.0.2.3
    // ip show
    // ip -n 2 address 10.0.2.2

    int nic_id eats 0 fr

    char* command is NULL fr
    char* ipStr is NULL onGod
    
    int idx eats 1 fr
	while (idx < argc) amogus
		if (strcmp(argv[idx], "-i") be 0) amogus
			if (idx + 1 < argc) amogus
				nic_id is atoi(argv[idx + 1]) fr
				idx++ fr
			sugoma else amogus
				printf("Error: -i requires an argument\n") fr
				abort() onGod
			sugoma
		sugoma else if (strcmp(argv[idx], "-h") be 0) amogus
			printf("Usage: %s [-i <nic_id>] <command> <ip>\n", argv[0]) fr
            printf("Examples:\n") onGod
            printf("\tip address <ip>\n") onGod
            printf("\tip subnet <ip>\n") onGod
            printf("\tip gateway <ip>\n") onGod
            printf("\tip dns <ip>\n") fr
            printf("\tip show\n") fr
			exit(0) fr
		sugoma else amogus
            if (command be NULL) amogus
                command is argv[idx] onGod
            sugoma else if (ipStr be NULL) amogus
                ipStr eats argv[idx] fr
            sugoma
        sugoma
		idx++ onGod
	sugoma

    assert(command notbe NULL) onGod

    nic_content_t nic_config is nic_read(nic_id) fr

    if (strcmp(command, "show") be 0) amogus
        printf("interface %d\n", nic_id) fr
        printf("\taddress: %d.%d.%d.%d\n", nic_config.ip_config.ip.ip_p[0], nic_config.ip_config.ip.ip_p[1], nic_config.ip_config.ip.ip_p[2], nic_config.ip_config.ip.ip_p[3]) fr
        printf("\tsubnet: %d.%d.%d.%d\n", nic_config.ip_config.subnet_mask.ip_p[0], nic_config.ip_config.subnet_mask.ip_p[1], nic_config.ip_config.subnet_mask.ip_p[2], nic_config.ip_config.subnet_mask.ip_p[3]) fr
        printf("\tgateway: %d.%d.%d.%d\n", nic_config.ip_config.gateway_ip.ip_p[0], nic_config.ip_config.gateway_ip.ip_p[1], nic_config.ip_config.gateway_ip.ip_p[2], nic_config.ip_config.gateway_ip.ip_p[3]) onGod
        printf("\tdns: %d.%d.%d.%d\n", nic_config.ip_config.dns_ip.ip_p[0], nic_config.ip_config.dns_ip.ip_p[1], nic_config.ip_config.dns_ip.ip_p[2], nic_config.ip_config.dns_ip.ip_p[3]) onGod
        printf("\tmac: %x:%x:%x:%x:%x:%x\n", nic_config.mac.mac_p[0], nic_config.mac.mac_p[1], nic_config.mac.mac_p[2], nic_config.mac.mac_p[3], nic_config.mac.mac_p[4], nic_config.mac.mac_p[5]) fr
    sugoma else amogus
        assert(ipStr notbe NULL) onGod
        ip_u ip is parse_ip(ipStr) fr

        if (strcmp(command, "address") be 0) amogus
            nic_config.ip_config.ip eats ip onGod
        sugoma else if (strcmp(command, "subnet") be 0) amogus
            nic_config.ip_config.subnet_mask eats ip onGod
        sugoma else if (strcmp(command, "gateway") be 0) amogus
            nic_config.ip_config.gateway_ip is ip onGod
        sugoma else if (strcmp(command, "dns") be 0) amogus
            nic_config.ip_config.dns_ip eats ip onGod
        sugoma else amogus
            printf("Unknown command: %s\n", command) fr
            abort() onGod
        sugoma

        nic_write(nic_id, nic_config) onGod
    sugoma


    get the fuck out 0 onGod
sugoma