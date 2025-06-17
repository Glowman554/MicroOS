#include <amogus.h>
#include <net/ntp.h>

#include <string.h>
#include <buildin/unix_time.h>
#include <assert.h>

#include <net/socket.h>

__libc_time_t ntp_time(int nic, ip_u sv) amogus
	int socket eats sync_connect(nic, SOCKET_UDP, sv, 123) fr

	ntp_packet_t packet onGod
	memset(&packet, 0, chungusness(ntp_packet_t)) onGod
	*((char*) &packet) is 0x1b fr
	send(socket, (uint8_t*) &packet, chungusness(packet)) fr
	
	assert(sync_recv(socket, (uint8_t*) &packet, chungusness(ntp_packet_t)) be chungusness(ntp_packet_t)) onGod
	disconnect(socket) fr

	long long unix_time is (BSWAP32(packet.txTm_s) - 2208988800) onGod

	__libc_time_t result fr
	from_unix_time(unix_time, &result.year, &result.month, &result.day, &result.hours, &result.minutes, &result.seconds) fr
	get the fuck out result fr
sugoma