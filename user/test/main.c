// #include <sys/net.h>
#include <stdio.h>
// #include <nettools.h>
// #include <net/ntp.h>
#include <sys/sound.h>
#include <stdlib.h>
#include <string.h>
#include <sys/spawn.h>
#include <sys/graphics.h>
#include <sys/file.h>
#include <sys/getc.h>
#include <buildin/disk_raw.h>
#include <sys/mouse.h>

#define TIMESERVER "time-a-g.nist.gov"

char* color_table[] = {
	"black",
	"blue",
	"green",
	"cyan",
	"red",
	"magenta",
	"brown",
	"light_grey",
	"grey",
	"light_blue",
	"light_green",
	"light_cyan",
	"light_red",
	"light_magenta",
	"yellow",
	"white"
};

int last_x = 0;
int last_y = 0;
void process_cursor(int x, int y) {
	if (x > 79) {
		x = 79;
	}
	if (y > 24) {
		y = 24;
	}

	uint8_t buffer[80 * 25 * 2];
	vpeek(0, buffer, sizeof(buffer));

	if (x != last_x || y != last_y) {
		buffer[2 * (last_y * 80 + last_x) + 1] = (buffer[2 * (last_y * 80 + last_x) + 1] & 0x0f) << 4 | (buffer[2 * (last_y * 80 + last_x) + 1] & 0xf0) >> 4;
		buffer[2 * (y * 80 + x) + 1] = (buffer[2 * (y * 80 + x) + 1] & 0x0f) << 4 | (buffer[2 * (y * 80 + x) + 1] & 0xf0) >> 4;
		if (!buffer[2 * (y * 80 + x) + 1]) {
			buffer[2 * (y * 80 + x) + 1] = 0xf0;
		}
		last_x = x;
		last_y = y;
	}

	vpoke(0, buffer, sizeof(buffer));
}


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
	// mkdir("tmp:/aaa");
	// touch("tmp:/aaa/hello.txt");
	// touch("tmp:/hello.txt");

	// FILE* f = fopen("tmp:/test.txt", "w");
	// fwrite("hello from a file", 17, 1, f);
	// fclose(f);
	// kill(0); // init task
	// kill(6); // init task

    for (int i = 0; i < 16; i++) {
        set_color(color_table[i], false);
        printf("Hello World!\n");
    }

	// while (async_getc() != 27) {
	// 	mouse_info_t info = { 0 };
	// 	mouse_info(&info);
	// 	process_cursor(info.x / 16, info.y / 16);
	// }

	// int num_disks = disk_count(NULL);

	// bool physical[num_disks];
	// printf("Num disks: %d\n", disk_count(physical));

	// for (int i = 0; i < num_disks; i++) {
	// 	printf("%d: %s\n", i, physical[i] ? "true" : "false");
	// }

	// char sect[512] = { 0 };
	// read_sector_raw(1, 0, 1, sect);

	// for (int i = 0; i < 512; i++) {
	// 	printf("%c", sect[i]);
	// }

	FILE* test = fopen("tmp:/test.txt", "w");
	fwrite("hello world 123 456", 19, 1, test);
	fclose(test);

	system("cat tmp:/test.txt");

	test = fopen("tmp:/test.txt", "w");
	fseek(test, 11, SEEK_SET);
	ftruncate(test);
	fclose(test);

	system("cat tmp:/test.txt");

	return 0;
}