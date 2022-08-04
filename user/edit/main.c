#include <sys/graphics.h>
#include <assert.h>
#include <render.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <input.h>

int main(int argc, char* argv[], char* envp[]) {
	if (argc != 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
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

	printf("buffer_ln_idx: %d\n", state.buffer_ln_idx);

	while (true) {
		render_tui(&state);
		if (listen_input(&state)) {
			break;
		}
	}

	free(state.input_buffer);
	fclose(state.file);
	return 0;
}