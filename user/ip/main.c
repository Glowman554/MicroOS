#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <nettools.h>
#include <net/ipv4.h>

int main(int argc, char* argv[]) {
    // EXAMPLE:
    // ip address 10.0.2.15
    // ip subnet 255.255.255.0
    // ip gateway 10.0.2.2
    // ip dns 10.0.2.3
    // ip show
    // ip -n 2 address 10.0.2.2

    int nic_id = 0;

    char* command = NULL;
    char* ipStr = NULL;
    
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
			printf("Usage: %s [-i <nic_id>] <command> <ip>\n", argv[0]);
            printf("Examples:\n");
            printf("\tip address <ip>\n");
            printf("\tip subnet <ip>\n");
            printf("\tip gateway <ip>\n");
            printf("\tip dns <ip>\n");
            printf("\tip show\n");
			exit(0);
		} else {
            if (command == NULL) {
                command = argv[idx];
            } else if (ipStr == NULL) {
                ipStr = argv[idx];
            }
        }
		idx++;
	}

    assert(command != NULL);

    nic_content_t nic_config = nic_read(nic_id);

    if (strcmp(command, "show") == 0) {
        printf("interface %d\n", nic_id);
        printf("\taddress: %d.%d.%d.%d\n", nic_config.ip_config.ip.ip_p[0], nic_config.ip_config.ip.ip_p[1], nic_config.ip_config.ip.ip_p[2], nic_config.ip_config.ip.ip_p[3]);
        printf("\tsubnet: %d.%d.%d.%d\n", nic_config.ip_config.subnet_mask.ip_p[0], nic_config.ip_config.subnet_mask.ip_p[1], nic_config.ip_config.subnet_mask.ip_p[2], nic_config.ip_config.subnet_mask.ip_p[3]);
        printf("\tgateway: %d.%d.%d.%d\n", nic_config.ip_config.gateway_ip.ip_p[0], nic_config.ip_config.gateway_ip.ip_p[1], nic_config.ip_config.gateway_ip.ip_p[2], nic_config.ip_config.gateway_ip.ip_p[3]);
        printf("\tdns: %d.%d.%d.%d\n", nic_config.ip_config.dns_ip.ip_p[0], nic_config.ip_config.dns_ip.ip_p[1], nic_config.ip_config.dns_ip.ip_p[2], nic_config.ip_config.dns_ip.ip_p[3]);
        printf("\tmac: %x:%x:%x:%x:%x:%x\n", nic_config.mac.mac_p[0], nic_config.mac.mac_p[1], nic_config.mac.mac_p[2], nic_config.mac.mac_p[3], nic_config.mac.mac_p[4], nic_config.mac.mac_p[5]);
    } else {
        assert(ipStr != NULL);
        ip_u ip = parse_ip(ipStr);

        if (strcmp(command, "address") == 0) {
            nic_config.ip_config.ip = ip;
        } else if (strcmp(command, "subnet") == 0) {
            nic_config.ip_config.subnet_mask = ip;
        } else if (strcmp(command, "gateway") == 0) {
            nic_config.ip_config.gateway_ip = ip;
        } else if (strcmp(command, "dns") == 0) {
            nic_config.ip_config.dns_ip = ip;
        } else {
            printf("Unknown command: %s\n", command);
            abort();
        }

        nic_write(nic_id, nic_config);
    }


    return 0;
}