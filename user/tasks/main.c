#include <buildin/graphics.h>
#include <sys/getc.h>
#include <buildin/ansi.h>
#include <sys/spawn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_TASKS 32
#define MAX(a, b) ((a > b) ? a : b)

int render_pids(task_list_t* list, int amount) {
	draw_string(0, 1, "pid", FOREGROUND_WHITE | BACKGROUND_BLACK);

	int max = 0;
	for (int i = 0; i < amount; i++) {
		char buf[16] = { 0 };
		int l = sprintf(buf, "%d", list[i].pid);
		if (l > max) {
			max = l;
		}
		draw_string(0, i + 2, buf, FOREGROUND_WHITE | BACKGROUND_BLACK);
	}
	return MAX(max, 3);
}

int render_terms(task_list_t* list, int amount, int offset) {
	draw_string(offset, 1, "term", FOREGROUND_WHITE | BACKGROUND_BLACK);

	int max = 0;
	for (int i = 0; i < amount; i++) {
		char buf[16] = { 0 };
		int l = sprintf(buf, "%d", list[i].term);
		if (l > max) {
			max = l;
		}
		draw_string(offset, i + 2, buf, FOREGROUND_WHITE | BACKGROUND_BLACK);
	}
	return MAX(max, 4);
}

int render_names(task_list_t* list, int amount, int offset) {
	draw_string(offset, 1, "name", FOREGROUND_WHITE | BACKGROUND_BLACK);
	for (int i = 0; i < amount; i++) {
		draw_string(offset, i + 2, list[i].name, FOREGROUND_WHITE | BACKGROUND_BLACK);
	}
}


void render_separator(int amount, int offset) {
	for (int i = 0; i < amount; i++) {
		draw_string(offset, i + 2, "|", FOREGROUND_WHITE | BACKGROUND_BLACK);
	}
}

int main(int argc, char* argv[]) {
	task_list_t* list = malloc(sizeof(task_list_t) * MAX_TASKS);

	char buffer[32] = { 0 };
	int idx = 0;

	char c = ' ';
	do {
		if (!c) {
			continue;
		}

		start_frame();
		
		draw_string(get_width() / 2 - 6, 0, "MicroOS Tasks", FOREGROUND_CYAN | BACKGROUND_BLACK);

		int amount = get_task_list(list, MAX_TASKS);

		int max = render_pids(list, amount);
		render_separator(amount, ++max);
		max += 2;
		max += render_terms(list, amount, max);
		render_separator(amount, ++max);
		max += 2;
		render_names(list, amount, max);

		draw_string(0, get_height() - 1, "kill > ", FOREGROUND_DARKGRAY | BACKGROUND_BLACK);

		if (c != ' ') {
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
		draw_string(7, get_height() - 1, buffer, FOREGROUND_DARKGRAY | BACKGROUND_BLACK);

		end_frame();
	} while ((c = async_getc()) != 27);

	free(list);
	ansi_printf("\033[H\033[J");
	return 0;
}