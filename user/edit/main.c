#include <sys/graphics.h>
#include <assert.h>
#include <render.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <input.h>
#include <buildin/ansi.h>

void print_usage(char* prog) {
	printf("Usage: %s <file-name>\n\n", prog);

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
	if (argc != 2) {
		print_usage(argv[0]);
		return 1;
	}

	if (!getenv("NOSYX")) {
		char syx[128] = { 0 };
		strcat(syx, getenv("ROOT_FS"));
		strcat(syx, "syntax/");
		strcat(syx, get_file_extension(argv[1]));
		strcat(syx, ".syx");
		syntax = load_syntax(syx);
	}

	
	edit_state_t state  = { 0 };

	state.file_name = argv[1];
	state.is_edited = false;
	state.is_in_insert_mode = true;

	state.file = fopen(argv[1], "r");
	if (state.file == NULL) {
		printf("Could not open file %s\n", argv[1]);
		return 1;
	}

	fsize(state.file, file_size);
	state.input_buffer = (char*) malloc(sizeof(char) * file_size);
	memset(state.input_buffer, 0, sizeof(char) * file_size);
	state.current_size = file_size;
	state.buffer_idx = file_size;
	fread(state.input_buffer, file_size, 1, state.file);

	for (int i = 0; i < state.current_size; i++) {
		state.char_cnt++;
		if (state.input_buffer[i] == '\n') {
			state.ln_cnt++;
			state.buffer_ln_idx++;
			state.char_cnt--;
		}
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