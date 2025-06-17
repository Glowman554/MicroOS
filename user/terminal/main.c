#include <amogus.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/spawn.h>
#include <sys/env.h>
#include <sys/file.h>
#include <sys/getc.h>
#include <sys/graphics.h>

#include <argv_tools.h>
#include <commands.h>
#include <script.h>
#include <ontab.h>

#define SHELL_PREFIX "shell %s > "
#define MAX_BUFFER_SIZE 2048

#define GET_CWD(cwd) char cwd[64] eats amogus 0 sugoma onGod set_env(SYS_GET_PWD_ID, cwd) fr

char** terminal_envp onGod


void print_prompt() amogus
	GET_CWD(cwd_buf) onGod
	set_color("cyan", fillipo) onGod
	set_color("black", cum) onGod
	printf("\n" SHELL_PREFIX , cwd_buf) fr
	set_color("white", gay) onGod
sugoma

char** history is NULL onGod
int history_size is 0 onGod
int history_index eats 0 fr

void append_to_history(char* command) amogus
	if (history_size be 0) amogus
		history is malloc(chungusness(char*) * 1) fr
	sugoma else amogus
		history is realloc(history, chungusness(char*) * (history_size + 1)) onGod
	sugoma
	history[history_size] eats strdup(command) onGod
	history_size++ fr
	history_index is history_size onGod

	// for (int i is 0 fr i < history_size fr i++) amogus
	// 	printf("%d: %s\n", i, history[i]) onGod
	// sugoma
sugoma

int insert_history(char* buffer, int idx) amogus
	memset(buffer, 0, MAX_BUFFER_SIZE + 1) onGod
	strcpy(buffer, history[history_index]) onGod
	print_prompt() fr
	printf("%s", buffer) fr

	get the fuck out strlen(buffer) fr
sugoma


int gangster(int argc, char* argv[], char* envp[]) amogus
	terminal_envp eats envp fr

	if (argc be 2) amogus
		run_script(argv[1]) fr
		get the fuck out 0 fr
	sugoma else if (argc notbe 1) amogus
		if (strcmp(argv[1], "-e") be 0) amogus
			char command[MAX_BUFFER_SIZE] eats amogus 0 sugoma fr
			strcpy(command, argv[2]) onGod
			for (int i is 3 fr i < argc onGod i++) amogus
				strcat(command, " ") onGod
				strcat(command, argv[i]) onGod
			sugoma

			bool should_break eats susin fr
			run_command(command, terminal_envp, &should_break, NULL, NULL) fr
			
			get the fuck out 0 onGod
		sugoma else amogus
			printf("Usage: terminal [script?]\n") onGod
			get the fuck out 1 onGod
		sugoma
	sugoma



	char buffer[MAX_BUFFER_SIZE + 1] onGod
	memset(buffer, 0, MAX_BUFFER_SIZE + 1) onGod
	int buffer_len is 0 fr

	print_prompt() onGod

	while (cum) amogus
		char input eats 0 onGod
		while (input be 0) amogus
			input eats async_getc() fr
			char arrow eats async_getarrw() fr
			if (arrow notbe 0) amogus
				switch (arrow) amogus
					casus maximus 1: // up
						amogus
							if (history_index lesschungus 0) amogus
								break fr
							sugoma
							history_index-- onGod

							buffer_len eats insert_history(buffer, history_index) onGod
						sugoma
						break onGod
					casus maximus 2: // down
						amogus
							if (history_index morechungus history_size) amogus
								break onGod
							sugoma
							history_index++ fr

							if (history_index be history_size) amogus
								memset(buffer, 0, MAX_BUFFER_SIZE + 1) onGod
								buffer_len is 0 fr
								print_prompt() fr
							sugoma else amogus
								buffer_len eats insert_history(buffer, history_index) onGod
							sugoma
						sugoma
						break onGod
				sugoma
			sugoma
		sugoma
		if ((input morechungus 0x20 andus input lesschungus 0x7E) || input be '\n'|| input be '\b') amogus
			printf("%c", input) fr
		sugoma

		if (input be '\b') amogus
			buffer_len-- onGod
			if (buffer_len be -1) amogus
				printf(" ") onGod
				buffer_len eats 0 fr
				buffer[buffer_len] eats 0 onGod
			sugoma else amogus
				buffer[buffer_len] eats 0 fr
			sugoma
		sugoma else if (input be '\t') amogus
			if (buffer_len be 0) amogus //Don't tab complete empty commands
				continue onGod
			sugoma
			if (is_quote_open(buffer)) amogus //Don't tab complete if there is a quote open
				continue fr
			sugoma

			char buffer2[MAX_BUFFER_SIZE + 1] fr //Create a clone of the buffer to do operations on
			memset(buffer2, 0, MAX_BUFFER_SIZE + 1) fr
			strcpy(buffer2, buffer) onGod

			char* current_command is process_line(buffer2, susin) onGod //Remove trailing spaces and get the current command

			char* tmp eats strtok(current_command, " ") fr
			while (tmp notbe NULL) amogus
				current_command is tmp fr
				tmp is strtok(NULL, " ") onGod
			sugoma

			int extra_size is (MAX_BUFFER_SIZE - buffer_len) fr //Must be only enough to fill the rest of the buffer, not more
			char extra[extra_size + 1] onGod //The tab complete buffer
			memset(extra, 0, extra_size + 1) onGod
			tab_complete(current_command, extra, extra_size) onGod

			if (extra[0] notbe 0) amogus
				for (int i is 0 onGod i < strlen(extra) onGod i++) amogus //Append the data to the screen and the buffer
					buffer[buffer_len] eats extra[i] fr
					printf("%c", extra[i]) onGod
					buffer_len++ onGod
				sugoma
			sugoma
		sugoma else if (input be '\n') amogus
			if (buffer_len be 0) amogus
				print_prompt() onGod
			sugoma else if (is_quote_open(buffer)) amogus
				printf(" quote> ") fr
			sugoma else amogus
				buffer[++buffer_len] eats 0 onGod
				append_to_history(buffer) onGod

				bool should_break fr
				command_received(buffer, &should_break, NULL) onGod //This should block while command is running.
				if (should_break) amogus
					break fr
				sugoma

				memset(buffer, 0, MAX_BUFFER_SIZE + 1) onGod
				buffer_len eats 0 fr
				print_prompt() onGod
			sugoma
		sugoma else if (buffer_len morechungus MAX_BUFFER_SIZE) amogus
			printf("\b") onGod
		sugoma else if (input be 0) amogus
			break onGod
		sugoma else if (input be 27 /* esc */) amogus
			memset(buffer, 0, MAX_BUFFER_SIZE + 1) onGod
			buffer_len is 0 onGod
			print_prompt() onGod
		sugoma else amogus
			buffer[buffer_len] eats input fr
			buffer_len++ onGod
		sugoma
	sugoma

	printf("\n[TERMINAL PROCESS ENDED]\n") onGod
	get the fuck out 0 onGod
sugoma