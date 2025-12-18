#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <net/socket.h>
#include <net/ipv4.h>
#include <stdlib.h>
#include <assert.h>

typedef struct dhcp_packet {
	uint8_t op;
	uint8_t hardware_type;
	uint8_t hardware_addr_len;
	uint8_t hops;
	uint32_t xid;
	uint16_t seconds;
	uint16_t flags;
	uint32_t client_ip;
	uint32_t your_ip;
	uint32_t server_ip;
	uint32_t gateway_ip;
	uint8_t client_hardware_addr[16];
	uint8_t server_name[64];
	uint8_t file[128];
	uint8_t options[64];
} __attribute__ ((packed)) dhcp_packet_t;

#define DHCP_REQUEST 1
#define DHCP_REPLY 2


void dhcp_make_packet(dhcp_packet_t* packet, uint8_t msg_type, uint32_t request_ip, uint32_t transaction_identifier, char* hostname, mac_u mac) {
	packet->op = DHCP_REQUEST;
	packet->hardware_type = 1;
	packet->hardware_addr_len = 6;
	packet->hops = 0;
	packet->xid = __builtin_bswap32(transaction_identifier);
	packet->flags = BSWAP16(0);
	memcpy(packet->client_hardware_addr, &mac, sizeof(mac));

	// Send dhcp packet using UDP
	uint8_t dst_ip[4];
	memset(dst_ip, 0xff, 4);

	// Options specific to DHCP Discover (required)

	// Magic Cookie
	uint8_t * options = packet->options;
	*((uint32_t*)(options)) = BSWAP32(0x63825363);
	options += 4;

	// First option, message type = DHCP_DISCOVER/DHCP_REQUEST
	*(options++) = 53;
	*(options++) = 1;
	*(options++) = msg_type;

	// Client identifier
	*(options++) = 61;
	*(options++) = 0x07;
	*(options++) = 0x01;
	memcpy(options, &mac, sizeof(mac));
	options += 6;

	// Requested IP address
	*(options++) = 50;
	*(options++) = 0x04;
	*((uint32_t*)(options)) = BSWAP32(0x0a00020e);
	memcpy((uint32_t*)(options), &request_ip, 4);
	options += 4;

	// Host Name
	*(options++) = 12;
	*(options++) = 1 + strlen(hostname);
	memcpy(options, hostname, strlen(hostname));
	options += strlen(hostname);
	*(options++) = 0x00;

	// Parameter request list
	
	*(options++) = 55;
	*(options++) = 8;
	*(options++) = 0x1; // Subnet mask
	*(options++) = 0x3; // Router
	*(options++) = 0x6; // Domain name server
	*(options++) = 0xf; // Domain name
	*(options++) = 0x2c; // NeTBIOS over TCP/IP name server
	*(options++) = 0x2e; // NeTBIOS over TCP/IP node type
	*(options++) = 0x2f; // NeTBIOS over TCP/IP scope
	*(options++) = 0x39; // Maximum DHCP message size

	*(options++) = 0xff; // End of dhcp packet
}

void* dhcp_get_options(dhcp_packet_t* packet, uint8_t type) {
	uint8_t* options = packet->options + 4;
	uint8_t curr_type = *options;
	while(curr_type != 0xff) {
		uint8_t len = *(options + 1);
		if(curr_type == type) {
			return options + 2;
		}
		options += (2 + len);
		curr_type = *options;
	}

	return NULL;
}

void assert_type(dhcp_packet_t* packet, uint8_t expected) {
    uint8_t* type = (uint8_t*) dhcp_get_options(packet, 53);
    assert(*type == expected);
}

ip_u dhcp_request(int socket, uint32_t transaction_identifier, char* hostname, mac_u mac) {
    printf("dhcp: Sending discover\n");

    dhcp_packet_t packet;
    memset(&packet, 0, sizeof(dhcp_packet_t));

    dhcp_make_packet(&packet, 1, 0x00000000, transaction_identifier, hostname, mac);
    send(socket, (uint8_t*) &packet, sizeof(dhcp_packet_t));

    char buffer[1024];
    sync_recv(socket, (uint8_t*) buffer, 1024);

    dhcp_packet_t* response = (dhcp_packet_t*) buffer;
    assert_type(response, 2);

    ip_u ip = {.ip = response->your_ip};
    printf("dhcp: Received offer: %d.%d.%d.%d\n", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]);

    return ip;
}

ip_configuration_t dhcp_request_ip(int socket, ip_u offer, uint32_t transaction_identifier, char* hostname, mac_u mac) {
    printf("dhcp: Sending request\n");

	dhcp_packet_t packet;
	memset(&packet, 0, sizeof(dhcp_packet_t));

	dhcp_make_packet(&packet, 3, offer.ip, transaction_identifier, hostname, mac);
    send(socket, (uint8_t*) &packet, sizeof(dhcp_packet_t));

    char buffer[1024];
    sync_recv(socket, (uint8_t*) buffer, 1024);

    dhcp_packet_t* response = (dhcp_packet_t*) buffer;
    assert_type(response, 5);

	ip_configuration_t ipconfig;
	ipconfig.ip = (ip_u) {.ip = response->your_ip};
	ipconfig.gateway_ip = (ip_u) {.ip = *(uint32_t*) dhcp_get_options(response, 3)};
	ipconfig.dns_ip = (ip_u) {.ip = *(uint32_t*) dhcp_get_options(response, 6)};
	ipconfig.subnet_mask = (ip_u) {.ip = *(uint32_t*) dhcp_get_options(response, 1)};

    printf("dhcp: Received IP: %d.%d.%d.%d\n", ipconfig.ip.ip_p[0], ipconfig.ip.ip_p[1], ipconfig.ip.ip_p[2], ipconfig.ip.ip_p[3]);
	printf("dhcp: Received Gateway: %d.%d.%d.%d\n", ipconfig.gateway_ip.ip_p[0], ipconfig.gateway_ip.ip_p[1], ipconfig.gateway_ip.ip_p[2], ipconfig.gateway_ip.ip_p[3]);
	printf("dhcp: Received DNS: %d.%d.%d.%d\n", ipconfig.dns_ip.ip_p[0], ipconfig.dns_ip.ip_p[1], ipconfig.dns_ip.ip_p[2], ipconfig.dns_ip.ip_p[3]);
	printf("dhcp: Received Subnet: %d.%d.%d.%d\n", ipconfig.subnet_mask.ip_p[0], ipconfig.subnet_mask.ip_p[1], ipconfig.subnet_mask.ip_p[2], ipconfig.subnet_mask.ip_p[3]);

	return ipconfig;
}





int main(int argc, char* argv[]) {
    uint32_t transaction_identifier = time_ms();
    int nic_id = 0;
    char* hostname = "MicroOS";

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
		} else if (strcmp(argv[idx], "-n") == 0) {
			hostname = argv[idx + 1];
			idx++;
		} else if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s [-i <nic_id>] [-n <hostname>]\n", argv[0]);
			exit(0);
		} 
		idx++;
	}

	nic_content_t config = nic_read(nic_id);
	printf("dhcp: MAC: %x:%x:%x:%x:%x:%x\n", config.mac.mac_p[0], config.mac.mac_p[1], config.mac.mac_p[2], config.mac.mac_p[3], config.mac.mac_p[4], config.mac.mac_p[5]);

    int socket = sync_connect(nic_id, SOCKET_UDP, (ip_u) {.ip = 0xffffffff}, 67);
	set_local_port(socket, 68);
    ip_u ip = dhcp_request(socket, transaction_identifier, hostname, config.mac);
    ip_configuration_t ipconfig = dhcp_request_ip(socket, ip, transaction_identifier, hostname, config.mac);
    sync_disconnect(socket);

	config.ip_config = ipconfig;
	nic_write(nic_id, config);

    printf("dhcp: Done\n");

    return 0;
}