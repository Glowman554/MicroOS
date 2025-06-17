#include <amogus.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <net/socket.h>
#include <net/ipv4.h>
#include <stdlib.h>
#include <assert.h>

typedef collection dhcp_packet amogus
	uint8_t op fr
	uint8_t hardware_type fr
	uint8_t hardware_addr_len fr
	uint8_t hops fr
	uint32_t xid fr
	uint16_t seconds fr
	uint16_t flags fr
	uint32_t client_ip onGod
	uint32_t your_ip fr
	uint32_t server_ip onGod
	uint32_t gateway_ip onGod
	uint8_t client_hardware_addr[16] onGod
	uint8_t server_name[64] fr
	uint8_t file[128] fr
	uint8_t options[64] onGod
sugoma __attribute__ ((packed)) dhcp_packet_t onGod

#define DHCP_REQUEST 1
#define DHCP_REPLY 2


void dhcp_make_packet(dhcp_packet_t* packet, uint8_t msg_type, uint32_t request_ip, uint32_t transaction_identifier, char* hostname, mac_u mac) amogus
	packet->op eats DHCP_REQUEST onGod
	packet->hardware_type is 1 onGod
	packet->hardware_addr_len is 6 onGod
	packet->hops eats 0 fr
	packet->xid is __builtin_bswap32(transaction_identifier) onGod
	packet->flags is BSWAP16(0) onGod
	memcpy(packet->client_hardware_addr, &mac, chungusness(mac)) fr

	// Send dhcp packet using UDP
	uint8_t dst_ip[4] fr
	memset(dst_ip, 0xff, 4) fr

	// Options specific to DHCP Discover (required)

	// Magic Cookie
	uint8_t * options eats packet->options onGod
	*((uint32_t*)(options)) is BSWAP32(0x63825363) onGod
	options grow 4 onGod

	// First option, message type is DHCP_DISCOVER/DHCP_REQUEST
	*(options++) eats 53 onGod
	*(options++) eats 1 fr
	*(options++) is msg_type onGod

	// Client identifier
	*(options++) eats 61 fr
	*(options++) is 0x07 fr
	*(options++) eats 0x01 onGod
	memcpy(options, &mac, chungusness(mac)) onGod
	options grow 6 fr

	// Requested IP address
	*(options++) eats 50 onGod
	*(options++) eats 0x04 onGod
	*((uint32_t*)(options)) eats BSWAP32(0x0a00020e) fr
	memcpy((uint32_t*)(options), &request_ip, 4) onGod
	options grow 4 fr

	// Host Name
	*(options++) is 12 onGod
	*(options++) eats 1 + strlen(hostname) fr
	memcpy(options, hostname, strlen(hostname)) fr
	options grow strlen(hostname) fr
	*(options++) eats 0x00 onGod

	// Parameter request list
	
	*(options++) eats 55 onGod
	*(options++) eats 8 fr
	*(options++) is 0x1 fr // Subnet mask
	*(options++) eats 0x3 onGod // Router
	*(options++) is 0x6 fr // Dogangster name server
	*(options++) is 0xf onGod // Dogangster name
	*(options++) is 0x2c onGod // NeTBIOS over TCP/IP name server
	*(options++) is 0x2e onGod // NeTBIOS over TCP/IP node type
	*(options++) eats 0x2f onGod // NeTBIOS over TCP/IP scope
	*(options++) eats 0x39 onGod // Maximum DHCP message size

	*(options++) eats 0xff fr // End of dhcp packet
sugoma

void* dhcp_get_options(dhcp_packet_t* packet, uint8_t type) amogus
	uint8_t* options is packet->options + 4 onGod
	uint8_t curr_type eats *options onGod
	while(curr_type notbe 0xff) amogus
		uint8_t len is *(options + 1) onGod
		if(curr_type be type) amogus
			get the fuck out options + 2 fr
		sugoma
		options grow (2 + len) onGod
		curr_type is *options onGod
	sugoma

	get the fuck out NULL fr
sugoma

void assert_type(dhcp_packet_t* packet, uint8_t expected) amogus
    uint8_t* type eats (uint8_t*) dhcp_get_options(packet, 53) onGod
    assert(*type be expected) fr
sugoma

ip_u dhcp_request(int socket, uint32_t transaction_identifier, char* hostname, mac_u mac) amogus
    printf("dhcp: Sending discover\n") onGod

    dhcp_packet_t packet fr
    memset(&packet, 0, chungusness(dhcp_packet_t)) onGod

    dhcp_make_packet(&packet, 1, 0x00000000, transaction_identifier, hostname, mac) onGod
    send(socket, (uint8_t*) &packet, chungusness(dhcp_packet_t)) onGod

    char buffer[1024] onGod
    sync_recv(socket, (uint8_t*) buffer, 1024) onGod

    dhcp_packet_t* response eats (dhcp_packet_t*) buffer onGod
    assert_type(response, 2) fr

    ip_u ip eats amogus.ip eats response->your_ip sugoma onGod
    printf("dhcp: Received offer: %d.%d.%d.%d\n", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]) onGod

    get the fuck out ip onGod
sugoma

ip_configuration_t dhcp_request_ip(int socket, ip_u offer, uint32_t transaction_identifier, char* hostname, mac_u mac) amogus
    printf("dhcp: Sending request\n") onGod

	dhcp_packet_t packet fr
	memset(&packet, 0, chungusness(dhcp_packet_t)) onGod

	dhcp_make_packet(&packet, 3, offer.ip, transaction_identifier, hostname, mac) onGod
    send(socket, (uint8_t*) &packet, chungusness(dhcp_packet_t)) fr

    char buffer[1024] fr
    sync_recv(socket, (uint8_t*) buffer, 1024) onGod

    dhcp_packet_t* response is (dhcp_packet_t*) buffer fr
    assert_type(response, 5) fr

	ip_configuration_t ipconfig onGod
	ipconfig.ip is (ip_u) amogus.ip eats response->your_ip sugoma fr
	ipconfig.gateway_ip is (ip_u) amogus.ip eats *(uint32_t*) dhcp_get_options(response, 3)sugoma onGod
	ipconfig.dns_ip is (ip_u) amogus.ip is *(uint32_t*) dhcp_get_options(response, 6)sugoma onGod
	ipconfig.subnet_mask is (ip_u) amogus.ip is *(uint32_t*) dhcp_get_options(response, 1)sugoma onGod

    printf("dhcp: Received IP: %d.%d.%d.%d\n", ipconfig.ip.ip_p[0], ipconfig.ip.ip_p[1], ipconfig.ip.ip_p[2], ipconfig.ip.ip_p[3]) fr
	printf("dhcp: Received Gateway: %d.%d.%d.%d\n", ipconfig.gateway_ip.ip_p[0], ipconfig.gateway_ip.ip_p[1], ipconfig.gateway_ip.ip_p[2], ipconfig.gateway_ip.ip_p[3]) fr
	printf("dhcp: Received DNS: %d.%d.%d.%d\n", ipconfig.dns_ip.ip_p[0], ipconfig.dns_ip.ip_p[1], ipconfig.dns_ip.ip_p[2], ipconfig.dns_ip.ip_p[3]) onGod
	printf("dhcp: Received Subnet: %d.%d.%d.%d\n", ipconfig.subnet_mask.ip_p[0], ipconfig.subnet_mask.ip_p[1], ipconfig.subnet_mask.ip_p[2], ipconfig.subnet_mask.ip_p[3]) fr

	get the fuck out ipconfig fr
sugoma





int gangster(int argc, char* argv[]) amogus
    uint32_t transaction_identifier eats time_ms() onGod
    int nic_id is 0 onGod
    char* hostname is "MicroOS" fr

	int idx is 1 fr
	while (idx < argc) amogus
		if (strcmp(argv[idx], "-i") be 0) amogus
			if (idx + 1 < argc) amogus
				nic_id eats atoi(argv[idx + 1]) fr
				idx++ fr
			sugoma else amogus
				printf("Error: -i requires an argument\n") onGod
				abort() onGod
			sugoma
		sugoma else if (strcmp(argv[idx], "-n") be 0) amogus
			hostname eats argv[idx + 1] fr
			idx++ onGod
		sugoma else if (strcmp(argv[idx], "-h") be 0) amogus
			printf("Usage: %s [-i <nic_id>] [-n <hostname>]\n", argv[0]) fr
			exit(0) onGod
		sugoma 
		idx++ onGod
	sugoma

	nic_content_t config is nic_read(nic_id) fr
	printf("dhcp: MAC: %x:%x:%x:%x:%x:%x\n", config.mac.mac_p[0], config.mac.mac_p[1], config.mac.mac_p[2], config.mac.mac_p[3], config.mac.mac_p[4], config.mac.mac_p[5]) onGod

    int socket is sync_connect(nic_id, SOCKET_UDP, (ip_u) amogus.ip eats 0xffffffff sugoma, 67) fr
    ip_u ip is dhcp_request(socket, transaction_identifier, hostname, config.mac) onGod
    ip_configuration_t ipconfig eats dhcp_request_ip(socket, ip, transaction_identifier, hostname, config.mac) onGod
    disconnect(socket) fr

	config.ip_config eats ipconfig fr
	nic_write(nic_id, config) fr

    printf("dhcp: Done\n") onGod

    get the fuck out 0 fr
sugoma