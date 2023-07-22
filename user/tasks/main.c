#include <buildin/graphics.h>
#include <sys/getc.h>
#include <buildin/ansi.h>
#include <sys/spawn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
	char c = 0;
	task_list_t* list = malloc(sizeof(task_list_t) * 10);

	char buffer[32] = { 0 };
	int idx = 0;

	while ((c = async_getc()) != 27) {
		start_frame();
		
		draw_string(40 - 6, 0, "MicroOS Tasks", FOREGROUND_CYAN | BACKGROUND_BLACK);

		int amount = get_task_list(list, 10);
		for (int i = 0; i < amount; i++) {
			char buf[16] = { 0 };
			sprintf(buf, "%d", list[i].pid);
			draw_string(0, i + 2, buf, FOREGROUND_WHITE | BACKGROUND_BLACK);
			draw_string(5, i + 2, list[i].name, FOREGROUND_WHITE | BACKGROUND_BLACK);
		}

		draw_string(0, 24, "kill > ", FOREGROUND_DARKGRAY | BACKGROUND_BLACK);
		draw_string(7, 24, buffer, FOREGROUND_DARKGRAY | BACKGROUND_BLACK);

		if (c) {
			if (c == '\b') {
				idx--;
				if (idx == -1) {
					idx = 0;
				}
				buffer[idx] = 0;
			} else if (c == '\n') {
				kill(atoi(buffer));
				memset(buffer, 0, sizeof(buffer));
				idx = 0;
			} else {
				buffer[idx++] = c;
			}
		}

		end_frame();
	}

	free(list);
	ansi_printf("\033[H\033[J");
	return 0;
}