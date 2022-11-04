#include <nettools.h>
#include <stdio.h>
#include <buildin/number_parser.h>

void format_ip(ip_u ip, char* out) {
	sprintf(out, "%d.%d.%d.%d", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]);
}

ip_u parse_ip(const char* in) {
	ip_u ip = { 0 };

	char* curr = (char*) in;
	for (int i = 0; i < 4; i++) {
		int r = 0;
		curr = __libc_parse_number(curr, &r);
		if ((*curr != '.' && *curr != 0) || (*curr == 0 && i != 3)) {
			return (ip_u) {.ip = 0};
		}
		curr++;
		ip.ip_p[i] = r;
	}

	return ip;
}