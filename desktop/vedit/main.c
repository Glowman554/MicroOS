#include <sys/graphics.h>
#include <erender.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <input.h>
#include <buildin/ansi.h>
#include <window.h>
#include <window/strinput.h>

char* get_file_extension(const char* filename) {
	char* chr_ptr = strchr(filename, '.');
	if (chr_ptr == NULL) {
		return "";
	}
	return ++chr_ptr;
}

psf1_font_t font;

int main(int argc, char* argv[], char* envp[]) {
	window_init(400, 700, 50, 50, "Editor");

    font = load_psf1_font("dev:font");

	char buffer[64] = { 0 };
	char* file;
	if (argc != 2) {
		strinput_t input = {
            .x = 7 * 8,
            .y = 0,
            .bgcolor = 0,
            .fgcolor = 0xffffffff,
            .buffer = buffer,
            .idx = 0
        };

		while (strinput(&font, &input)) {
			window_optimize();

			if (window_redrawn()) {
				window_clear(0);
				draw_string_window(&font, 0, 0, "path > ", 0xffffffff, 0);
			}
		}
        file = buffer;
	} else {
		file = argv[1];
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

	state.file_name = file;
	state.is_edited = false;
	state.is_in_insert_mode = true;

	state.file = fopen(argv[1], "r");
	if (state.file == NULL) {
		// printf("Could not open file %s\n", argv[1]);
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
		render_ui(&state);
		if (listen_input(&state)) {
			break;
		}
	}

	free(state.input_buffer);
	fclose(state.file);

	return 0;
}