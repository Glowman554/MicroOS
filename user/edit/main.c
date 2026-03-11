#include <sys/graphics.h>
#include <assert.h>
#include <render.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <input.h>
#include <buildin/ansi.h>

void print_usage(char* prog) {
	printf("Usage: %s [-r] <file-name>\n\n", prog);

	printf("Cheat sheet\n");
	printf("w        -> move up a line\n");
	printf("a        -> move left a character\n");
	printf("s        -> move down a line\n");
	printf("d        -> move right a char\n");
	printf("+        -> write changes to file\n");
	printf("q        -> quit\n");

	printf("\n");
	printf("Those commands work in the EDIT mode witch can be activated by pressing 'esc'.\n");
	printf("To exit EDIT mode press 'esc' again.\n");
	printf("To disable syntax highlighting set the 'NOSYX' env variable to something.\n");
}

char* get_file_extension(const char* filename) {
	char* chr_ptr = strchr(filename, '.');
	if (chr_ptr == NULL) {
		return "";
	}
	return ++chr_ptr;
}

int main(int argc, char* argv[], char* envp[]) {
	bool read_only = false;
	char* file_name = NULL;

	int idx = 1;
	while (idx < argc) {
		if (strcmp(argv[idx], "-r") == 0) {
			read_only = true;
		} else if (strcmp(argv[idx], "-h") == 0) {
			print_usage(argv[0]);
			return 0;
		} else {
			if (file_name == NULL) {
				file_name = argv[idx];
			} else {
				printf("Error: Too many arguments\n");
				print_usage(argv[0]);
				return 1;
			}
		}

		idx++;
	}


	if (!getenv("NOSYX")) {
		char syx[128] = { 0 };
		strcat(syx, getenv("ROOT_FS"));
		strcat(syx, "syntax/");
		strcat(syx, get_file_extension(file_name));
		strcat(syx, ".syx");
		syntax = load_syntax(syx);
	}

	
	edit_state_t state  = { 0 };

	state.file_name = file_name;
	state.is_edited = false;
	state.is_in_insert_mode = true;
	state.read_only = read_only;

	state.file = fopen(file_name, "r");
	if (state.file == NULL) {
		printf("Could not open file %s\n", file_name);
		return 1;
	}

	fsize(state.file, file_size);
	state.input_buffer = (char*) malloc(sizeof(char) * file_size);
	memset(state.input_buffer, 0, sizeof(char) * file_size);
	state.current_size = file_size;
	state.buffer_idx = file_size;

	if (file_size) {
		fread(state.input_buffer, file_size, 1, state.file);
	}

	state.ln_cnt = 1;
	for (int i = 0; i < state.current_size; i++) {
		state.char_cnt++;
		if (state.input_buffer[i] == '\n') {
			state.ln_cnt++;
			state.buffer_ln_idx++;
			state.char_cnt--;
		}
	}

	if (read_only) {
		state.buffer_idx = 0;
		state.buffer_ln_idx = 0;
	}

	while (true) {
		render_tui(&state);
		if (listen_input(&state)) {
			break;
		}
	}

	free(state.input_buffer);
	fclose(state.file);
	ansi_printf("\033[H\033[J");
	return 0;
}