// #include <sys/net.h>
#include <stdio.h>
// #include <nettools.h>
// #include <net/ntp.h>
#include <sys/sound.h>
#include <stdlib.h>
#include <string.h>
#include <sys/spawn.h>
#include <sys/graphics.h>

#define TIMESERVER "time-a-g.nist.gov"

int main(int argc, char* argv[], char* envp[]) {
	// int nic_id = 0;
	// ip_u ip = dns_resolve_A(nic_id, argv[1]);
	// ip_u ip = parse_ip("10.0.2.2");
	
	// char out[0xff] = { 0 };
	// format_ip(ip, out);
	// printf("ip: %s\n", out);

	// printf("ping answerd: %s", icmp_ping(nic_id, ip) ? "true" : "false");

	// __libc_time_t time = ntp_time(nic_id);

	// char out[0xff] = { 0 };
	// time_format(out, &time);
	// printf("it is %s\n", out);

	// int sock = connect(nic_id, SOCKET_UDP, parse_ip("10.0.2.2"), 1234);
	// printf("Got socket id %d\n", sock);
	// send(sock, (uint8_t*) "Hello wordl!", 13);
	// disconnect(sock);

	// __libc_time_t time = ntp_time(nic_id, dns_resolve_A(nic_id, TIMESERVER));
	// char out[0xff] = { 0 };
	// time_format(out, &time);
	// printf("It is %s\n", out);

	// sound_context_t* context = malloc(sizeof(sound_context_t) + (sizeof(queued_note_t) * 85));
	// memset(context, 0, sizeof(sound_context_t) + (sizeof(queued_note_t) * 85));

	// int idx = 0;
	// for (int i = 0; i < 7; i++) {
	// 	for (int j = 0; j < 12; j++) {
	// 		context->notes[idx++] = (queued_note_t) {
	// 			.note = ENCODE_NOTE(i, j),
	// 			.duration_ms = 100
	// 		};
	// 	}
	// }
	// context->num_notes = idx;
	// printf("%d notes!\n", idx);

	// coro_t coro = { 0 };
	// while (async_getc() != 27) {
	// 	sound_run(&coro, context);
	// }

	// free(context);

	// sound_clear(0);

	// set_color("green", false);
	// set_color("white", true);
	// task_list_t* list = malloc(sizeof(task_list_t) * 10);
	// int m = get_task_list(list, 10);
	// for (int i = 0; i < m; i++) {
	// 	printf("%d: %s\n", list[i].pid, list[i].name);
	// }
	while (1);
}