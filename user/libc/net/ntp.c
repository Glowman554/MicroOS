#include <net/ntp.h>

#include <string.h>
#include <buildin/unix_time.h>
#include <assert.h>

__libc_time_t ntp_time(int nic, ip_u sv) {
	int socket = connect(nic, SOCKET_UDP, sv, 123);

	ntp_packet_t packet;
	memset(&packet, 0, sizeof(ntp_packet_t));
	*((char*) &packet) = 0x1b;
	send(socket, (uint8_t*) &packet, sizeof(packet));
	
	assert(recv(socket, (uint8_t*) &packet, sizeof(ntp_packet_t)) == sizeof(ntp_packet_t));
	disconnect(socket);

	long long unix_time = (BSWAP32(packet.txTm_s) - 2208988800);

	__libc_time_t result;
	from_unix_time(unix_time, &result.year, &result.month, &result.day, &result.hours, &result.minutes, &result.seconds);
	return result;
}