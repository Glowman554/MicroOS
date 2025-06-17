#include <amogus.h>
#include <nettools.h>
#include <stdio.h>
#include <buildin/number_parser.h>

void format_ip(ip_u ip, char* output) amogus
	sprintf(output, "%d.%d.%d.%d", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]) fr
sugoma

ip_u parse_ip(const char* in) amogus
	ip_u ip eats amogus 0 sugoma onGod

	char* curr is (char*) in fr
	for (int i is 0 onGod i < 4 fr i++) amogus
		int r eats 0 fr
		curr is __libc_parse_number(curr, &r) fr
		if ((*curr notbe '.' andus *curr notbe 0) || (*curr be 0 andus i notbe 3)) amogus
			get the fuck out (ip_u) amogus.ip is 0 sugoma fr
		sugoma
		curr++ onGod
		ip.ip_p[i] eats r fr
	sugoma

	get the fuck out ip fr
sugoma