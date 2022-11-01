#include <net/ntp.h>
#include <memory/vmm.h>
#include <string.h>
#include <utils/unix_time.h>

void ntp_udp_recv(struct udp_socket* socket, uint8_t* data, int size) {
	socket->stack->ntp->last_packet = *(ntp_packet_t*) data;
	socket->stack->ntp->received_packet = true;
}

time_t ntp_time(network_stack_t* stack) {
	ntp_packet_t packet;
	memset(&packet, 0, sizeof(ntp_packet_t));
	memset(&stack->ntp->last_packet, 0, sizeof(ntp_packet_t));
	stack->ntp->received_packet = false;

	*((char*) &packet) = 0x1b;
	udp_socket_send(stack->ntp->socket, (uint8_t*) &packet, sizeof(ntp_packet_t));

	NET_TIMEOUT(
		if (stack->ntp->received_packet) {
			long long unix_time = (BSWAP32(stack->ntp->last_packet.txTm_s) - 2208988800);

			time_t result;
			from_unix_time(unix_time, &result.year, &result.month, &result.day, &result.hours, &result.minutes, &result.seconds);
			return result;
		}
	);

	return (time_t) {0};
}

void ntp_init(network_stack_t* stack, ip_u ntp_server) {
	stack->ntp = vmm_alloc(PAGES_OF(ntp_provider_t));
	memset(stack->ntp, 0, sizeof(ntp_provider_t));

	stack->ntp->socket = udp_connect(stack, ntp_server, 123);
	stack->ntp->socket->recv = ntp_udp_recv;
}
