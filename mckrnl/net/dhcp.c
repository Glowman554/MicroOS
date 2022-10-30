#include <net/dhcp.h>

#include <memory/vmm.h>
#include <string.h>
#include <stddef.h>

void dhcp_request(network_stack_t* stack) {
	dhcp_packet_t packet;
	memset(&packet, 0, sizeof(dhcp_packet_t));

	dhcp_make_packet(stack, &packet, 1, 0x00000000);
	udp_socket_send(stack->dhcp->socket, (uint8_t*) &packet, sizeof(dhcp_packet_t));

    NET_TIMEOUT(
        if (stack->dhcp->completed) {
            return;
        }
    );
}

void dhcp_request_ip(network_stack_t* stack, ip_u ip) {
    dhcp_packet_t packet;
	memset(&packet, 0, sizeof(dhcp_packet_t));

	dhcp_make_packet(stack, &packet, 3, ip.ip);
	udp_socket_send(stack->dhcp->socket, (uint8_t*) &packet, sizeof(dhcp_packet_t));
}

void dhcp_make_packet(network_stack_t* stack, dhcp_packet_t* packet, uint8_t msg_type, uint32_t request_ip) {
	packet->op = DHCP_REQUEST;
	packet->hardware_type = 1;
	packet->hardware_addr_len = 6;
	packet->hops = 0;
	packet->xid = BSWAP32(DHCP_TRANSACTION_IDENTIFIER);
	packet->flags = BSWAP16(0);
	uint64_t mac = stack->driver->mac.mac;
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
	*(options++) = 1 + strlen(HOSTNAME);
	memcpy(options, HOSTNAME, strlen(HOSTNAME));
	options += strlen(HOSTNAME);
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

void dhcp_udp_recv(struct udp_socket* socket, uint8_t* data, int size) {
    dhcp_packet_t* packet = (dhcp_packet_t*) data;

	uint8_t* type = (uint8_t*) dhcp_get_options(packet, 53);

	switch (*type) {
		case 2:
			dhcp_request_ip(socket->stack, (ip_u) {.ip = packet->your_ip});
			break;
		case 5:
			socket->stack->dhcp->ip = (ip_u) {.ip = packet->your_ip};
			socket->stack->dhcp->gateway = (ip_u) {.ip = *(uint32_t*) dhcp_get_options(packet, 3)};
			socket->stack->dhcp->dns = (ip_u) {.ip = *(uint32_t*) dhcp_get_options(packet, 6)};
			socket->stack->dhcp->completed = true;
			socket->stack->dhcp->subnet = (ip_u) {.ip = *(uint32_t*) dhcp_get_options(packet, 1)};
			break;
	}
}

void dhcp_init(network_stack_t* stack) {
    stack->dhcp = vmm_alloc(PAGES_OF(dhpc_provider_t));
    memset(stack->dhcp, 0, sizeof(dhpc_provider_t));

    stack->dhcp->socket = udp_connect(stack, (ip_u) {.ip = 0xffffffff}, 67);
    stack->dhcp->socket->local_port = BSWAP16(68);
    stack->dhcp->socket->recv = dhcp_udp_recv;
}
