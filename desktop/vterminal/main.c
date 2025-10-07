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

#include <window.h>
#include <window/vconsole.h>

#define SHELL_PREFIX "shell %s > "
#define MAX_BUFFER_SIZE 2048

#define GET_CWD(cwd) char cwd[64] = { 0 }; set_env(SYS_GET_PWD_ID, cwd);

char** terminal_envp;


void print_prompt() {
	GET_CWD(cwd_buf);
	vconsole_set_color(0, true);
	vconsole_set_color(0xFF00AAAA, false);
	
	char prompt[64] = { 0 };
	sprintf(prompt, "\n" SHELL_PREFIX , cwd_buf);

	vconsole_puts(prompt);
	
	vconsole_set_color(0xffffffff, false);
}

char** history = NULL;
int history_size = 0;
int history_index = 0;

void append_to_history(char* command) {
	if (history_size == 0) {
		history = malloc(sizeof(char*) * 1);
	} else {
		history = realloc(history, sizeof(char*) * (history_size + 1));
	}
	history[history_size] = strdup(command);
	history_size++;
	history_index = history_size;

	// for (int i = 0; i < history_size; i++) {
	// 	printf("%d: %s\n", i, history[i]);
	// }
}

int insert_history(char* buffer, int idx) {
	memset(buffer, 0, MAX_BUFFER_SIZE + 1);
	strcpy(buffer, history[history_index]);
	print_prompt();
	vconsole_puts(buffer);

	return strlen(buffer);
}


int main(int argc, char* argv[], char* envp[]) {
	window_init(200, 400, 50, 50, "Terminal");

    psf1_font_t font = load_psf1_font("dev:font");
	vconsole_init(&font);

	terminal_envp = envp;
	
	export("export VTERM=1");

	if (argc == 2) {
		run_script(argv[1]);
		return 0;
	} else if (argc != 1) {
		if (strcmp(argv[1], "-e") == 0) {
			char command[MAX_BUFFER_SIZE] = { 0 };
			strcpy(command, argv[2]);
			for (int i = 3; i < argc; i++) {
				strcat(command, " ");
				strcat(command, argv[i]);
			}

			bool should_break = false;
			run_command(command, terminal_envp, &should_break, NULL, NULL);
			
			return 0;
		} else {
			vconsole_puts("Usage: terminal [script?]\n");
			return 1;
		}
	}



	char buffer[MAX_BUFFER_SIZE + 1];
	memset(buffer, 0, MAX_BUFFER_SIZE + 1);
	int buffer_len = 0;

	print_prompt();

	while (true) {
		window_optimize();

		char input = 0;
		while (input == 0) {
			input = window_async_getc();
			char arrow = window_async_getarrw();
			if (arrow != 0) {
				switch (arrow) {
					case 1: // up
						{
							if (history_index <= 0) {
								break;
							}
							history_index--;

							buffer_len = insert_history(buffer, history_index);
						}
						break;
					case 2: // down
						{
							if (history_index >= history_size) {
								break;
							}
							history_index++;

							if (history_index == history_size) {
								memset(buffer, 0, MAX_BUFFER_SIZE + 1);
								buffer_len = 0;
								print_prompt();
							} else {
								buffer_len = insert_history(buffer, history_index);
							}
						}
						break;
				}
			}
		}
		if ((input >= 0x20 && input <= 0x7E) || input == '\n'|| input == '\b') {
			vconsole_putc(input);
		}

		if (input == '\b') {
			buffer_len--;
			if (buffer_len == -1) {
				vconsole_putc(' ');
				buffer_len = 0;
				buffer[buffer_len] = 0;
			} else {
				buffer[buffer_len] = 0;
			}
		} else if (input == '\t') {
			if (buffer_len == 0) { //Don't tab complete empty commands
				continue;
			}
			if (is_quote_open(buffer)) { //Don't tab complete if there is a quote open
				continue;
			}

			char buffer2[MAX_BUFFER_SIZE + 1]; //Create a clone of the buffer to do operations on
			memset(buffer2, 0, MAX_BUFFER_SIZE + 1);
			strcpy(buffer2, buffer);

			char* current_command = process_line(buffer2, false); //Remove trailing spaces and get the current command

			char* tmp = strtok(current_command, " ");
			while (tmp != NULL) {
				current_command = tmp;
				tmp = strtok(NULL, " ");
			}

			int extra_size = (MAX_BUFFER_SIZE - buffer_len); //Must be only enough to fill the rest of the buffer, not more
			char extra[extra_size + 1]; //The tab complete buffer
			memset(extra, 0, extra_size + 1);
			tab_complete(current_command, extra, extra_size);

			if (extra[0] != 0) {
				for (int i = 0; i < strlen(extra); i++) { //Append the data to the screen and the buffer
					buffer[buffer_len] = extra[i];
					vconsole_putc(extra[i]);
					buffer_len++;
				}
			}
		} else if (input == '\n') {
			if (buffer_len == 0) {
				print_prompt();
			} else if (is_quote_open(buffer)) {
				vconsole_puts(" quote> ");
			} else {
				buffer[++buffer_len] = 0;
				append_to_history(buffer);

				bool should_break;
				command_received(buffer, &should_break, NULL); //This should block while command is running.
				if (should_break) {
					break;
				}

				memset(buffer, 0, MAX_BUFFER_SIZE + 1);
				buffer_len = 0;
				print_prompt();
			}
		} else if (buffer_len >= MAX_BUFFER_SIZE) {
			printf("\b");
		} else if (input == 0) {
			break;
		} else if (input == 27 /* esc */) {
			memset(buffer, 0, MAX_BUFFER_SIZE + 1);
			buffer_len = 0;
			print_prompt();
		} else {
			buffer[buffer_len] = input;
			buffer_len++;
		}
	}

	vconsole_puts("\n[TERMINAL PROCESS ENDED]\n");
	return 0;
}