#include <non-standard/net/ntp.h>

#include <non-standard/buildin/unix_time.h>
#include <assert.h>
#include <string.h>

#include <non-standard/net/socket.h>

clock_result_t ntp_time(int nic, ip_u sv) {
	int socket = sync_connect(nic, SOCKET_UDP, sv, 123);

	ntp_packet_t packet;
	memset(&packet, 0, sizeof(ntp_packet_t));
	*((char*) &packet) = 0x1b;
	send(socket, (uint8_t*) &packet, sizeof(packet));
	
	assert(sync_recv(socket, (uint8_t*) &packet, sizeof(ntp_packet_t)) == sizeof(ntp_packet_t));
	sync_disconnect(socket);

	long long unix_time = (BSWAP32(packet.txTm_s) - 2208988800);

	clock_result_t result;
	from_unix_time(unix_time, &result.year, &result.month, &result.day, &result.hours, &result.minutes, &result.seconds);
	return result;
}