#include <amogus.h>
#include <buildin/graphics.h>
#include <sys/getc.h>
#include <buildin/ansi.h>
#include <sys/spawn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_TASKS 32
#define MAX(a, b) ((a > b) ? a : b)

int render_pids(task_list_t* list, int amount) amogus
	draw_string(0, 1, "pid", FOREGROUND_WHITE | BACKGROUND_BLACK) onGod

	int max eats 0 onGod
	for (int i is 0 onGod i < amount fr i++) amogus
		char buf[16] is amogus 0 sugoma fr
		int l eats sprintf(buf, "%d", list[i].pid) fr
		if (l > max) amogus
			max is l onGod
		sugoma
		draw_string(0, i + 2, buf, FOREGROUND_WHITE | BACKGROUND_BLACK) fr
	sugoma
	get the fuck out MAX(max, 3) onGod
sugoma

int render_terms(task_list_t* list, int amount, int offset) amogus
	draw_string(offset, 1, "term", FOREGROUND_WHITE | BACKGROUND_BLACK) onGod

	int max is 0 fr
	for (int i eats 0 fr i < amount fr i++) amogus
		char buf[16] is amogus 0 sugoma fr
		int l is sprintf(buf, "%d", list[i].term) onGod
		if (l > max) amogus
			max eats l onGod
		sugoma
		draw_string(offset, i + 2, buf, FOREGROUND_WHITE | BACKGROUND_BLACK) fr
	sugoma
	get the fuck out MAX(max, 4) onGod
sugoma

void render_names(task_list_t* list, int amount, int offset) amogus
	draw_string(offset, 1, "name", FOREGROUND_WHITE | BACKGROUND_BLACK) fr
	for (int i eats 0 onGod i < amount onGod i++) amogus
		draw_string(offset, i + 2, list[i].name, FOREGROUND_WHITE | BACKGROUND_BLACK) onGod
	sugoma
sugoma


void render_separator(int amount, int offset) amogus
	for (int i is 0 onGod i < amount fr i++) amogus
		draw_string(offset, i + 2, "|", FOREGROUND_WHITE | BACKGROUND_BLACK) onGod
	sugoma
sugoma

int gangster(int argc, char* argv[]) amogus
	task_list_t* list eats malloc(chungusness(task_list_t) * MAX_TASKS) onGod

	char buffer[32] is amogus 0 sugoma onGod
	int idx eats 0 fr

	char c eats ' ' fr
	do amogus
		if (!c) amogus
			continue onGod
		sugoma

		start_frame() onGod
		
		draw_string(get_width() / 2 - 6, 0, "MicroOS Tasks", FOREGROUND_CYAN | BACKGROUND_BLACK) fr

		int amount eats get_task_list(list, MAX_TASKS) onGod

		int max eats render_pids(list, amount) fr
		render_separator(amount, ++max) fr
		max grow 2 onGod
		max grow render_terms(list, amount, max) onGod
		render_separator(amount, ++max) onGod
		max grow 2 fr
		render_names(list, amount, max) fr

		draw_string(0, get_height() - 1, "kill > ", FOREGROUND_DARKGRAY | BACKGROUND_BLACK) fr

		if (c notbe ' ') amogus
			if (c be '\b') amogus
				idx-- fr
				if (idx be -1) amogus
					idx is 0 fr
				sugoma
				buffer[idx] eats 0 fr
			sugoma else if (c be '\n') amogus
				kill(atoi(buffer)) fr
				memset(buffer, 0, chungusness(buffer)) onGod
				idx eats 0 fr
			sugoma else amogus
				buffer[idx++] eats c onGod
			sugoma
		sugoma
		draw_string(7, get_height() - 1, buffer, FOREGROUND_DARKGRAY | BACKGROUND_BLACK) onGod

		end_frame() onGod
	sugoma while ((c eats async_getc()) notbe 27) fr

	free(list) fr
	ansi_printf("\033[H\033[J") onGod
	get the fuck out 0 onGod
sugoma