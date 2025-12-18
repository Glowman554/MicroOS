#include <net/socket.h>
#include <net/dns.h>

#include <async.h>
#include <buildin/time.h>

#include <stdio.h>
#include <string.h>

static int str_copy_trunc(char* dst, int dst_size, const char* src) {
	if (dst == NULL || src == NULL || dst_size <= 0) {
		return 0;
	}

	int n = strnlen(src, dst_size - 1);
	memcpy(dst, src, n);
	dst[n] = 0;
	return n;
}

int main(int argc, char* argv[], char* envp[]) {
	(void)envp;

	if (argc != 3) {
		printf("Usage: %s <host> <path>\n", argv[0]);
		printf("Example: %s toxicfox.de /\n", argv[0]);
		return 1;
	}

	int nic = 0;
	uint16_t port = 80;

	ip_u ip = parse_ip(argv[1]);
	if (ip.ip == 0) {
		ip = dns_resolve_A(nic, (char*)argv[1]);
	}

	if (ip.ip == 0) {
		printf("httpget: could not resolve %s\n", argv[1]);
		return 1;
	}

	printf("httpget: connecting to %d.%d.%d.%d:%d\n", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3], port);
	int sock = sync_connect(nic, SOCKET_TCP, ip, port);

	char req[512];
	int req_len = sprintf(req,
		"GET %s HTTP/1.1\r\n"
		"Host: %s\r\n"
		"User-Agent: MicroOS-httpget\r\n"
		"Connection: close\r\n"
		"\r\n",
		argv[2], argv[1]
	);

	send(sock, (uint8_t*)req, req_len);

	async_t async = { .state = STATE_INIT };
	uint8_t buf[512];
	int idle_ticks = 0;

	while (1) {
		int n = recv(sock, &async, buf, (int)sizeof(buf) - 1);
		if (n > 0) {
            for (int i = 0; i < n; i++) {
                putchar(buf[i]);
            }

            async.state = STATE_INIT;
			idle_ticks = 0;
			continue;
		}

		sleep_ms(1);
		if (++idle_ticks > 300) {
			break;
		}
	}

	sync_disconnect(sock);
	return 0;
}
