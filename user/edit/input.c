#include <input.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/getc.h>

#include <render.h>

void move_up(edit_state_t* state) {
	if (state->buffer_ln_idx <= 0 || state->buffer_idx <= 0) {
	} else {
		// move one line up
		int prev_buff = state->buffer_idx;

		for (int i = state->buffer_idx; i > 0; i--) {
			state->buffer_idx--;
			if (state->input_buffer[i - 1] == '\n' || state->buffer_idx < 0) {
				break;
			}
		}
		if (state->buffer_idx < 0) {
			state->buffer_idx = prev_buff;
		} else {
			state->buffer_ln_idx--;
		}
	}
}

void move_down(edit_state_t* state) {
	if (state->buffer_ln_idx >= state->ln_cnt - 1 || state->buffer_idx >= state->current_size) {
	} else {
		// move one line up
		int prev_buff = state->buffer_idx;

		for (int i = state->buffer_idx; i < state->current_size; i++) {
			state->buffer_idx++;
			if (state->input_buffer[i] == '\n' || state->buffer_idx > state->current_size) {
				break;
			}
		}
		if (state->buffer_idx > state->current_size) {
			state->buffer_idx = prev_buff;
		} else {
			state->buffer_ln_idx++;
		}
	}
}

void move_left(edit_state_t* state) {
	if (!(state->buffer_idx <= 0)) {
		if (state->input_buffer[state->buffer_idx - 1] == '\n') {
			state->buffer_ln_idx--;
		}
		state->buffer_idx -= 1;
	}
}

void move_right(edit_state_t* state) {
	if (state->buffer_idx < state->current_size) {
		if (state->input_buffer[state->buffer_idx] == '\n') {
			state->buffer_ln_idx++;
		}
		state->buffer_idx += 1;
	}
}


bool listen_input(edit_state_t* state) {
	char input = 0;
	while ((input = async_getc()) == 0) {
		switch (async_getarrw()) {
			case 1:
				move_up(state);
				return false;
			case 2:
				move_down(state);
				return false;
			case 3:
				move_left(state);
				return false;
			case 4:
				move_right(state);
				return false;
		}
	}



	if (!state->is_in_insert_mode) {
		switch (input) {
			case 'q':
				return true;
			case '\e':
				state->is_in_insert_mode = !state->is_in_insert_mode;
				break;

			case 'a':
				move_left(state);
				break;
			case 'd':
				move_right(state);
				break;
			case 'w':
				move_up(state);
				break;
			case 's':
				move_down(state);
				break;

			case '+':
				state->file = freopen(state->file_name, "w", state->file);
				fseek(state->file, 0, SEEK_SET);
				fwrite(state->input_buffer, state->current_size, 1, state->file);
				fseek(state->file, state->current_size, SEEK_SET);
				ftruncate(state->file);
				state->is_edited = false;
				break;
		}
	} else {
		switch (input) {
			case '\b': {
				if (state->buffer_idx - 2 < 0 || state->current_size - 1 < 0) {
				} else {
					if (state->input_buffer[state->buffer_idx - 1] == '\n') {
						state->buffer_ln_idx--;
					}

					if (state->buffer_idx == state->current_size) {
					} else {
					   memmove((void*) &state->input_buffer[state->buffer_idx - 1], (void*) &state->input_buffer[state->buffer_idx], (state->current_size - state->buffer_idx) * sizeof(char));
					}
					if (state->input_buffer[state->buffer_idx] == '\n') {
						state->ln_cnt--;
					} else {
						state->char_cnt--;
					}
					state->input_buffer = (char*) realloc((void*) state->input_buffer, --state->current_size);
					state->buffer_idx--;

					rerender_color(state);
				}
			}
			break;

			case '\e': {
				state->is_in_insert_mode = !state->is_in_insert_mode;
			}
			break;


			default: {
				if (input == '\n') {
					state->ln_cnt++;
					state->buffer_ln_idx++;
				} else {
					state->char_cnt++;
				}

				state->is_edited = true;
				state->input_buffer = (char*) realloc((void*) state->input_buffer, ++state->current_size);
				memmove((void*) &state->input_buffer[state->buffer_idx+1], (void*) &state->input_buffer[state->buffer_idx], (state->current_size - state->buffer_idx) * sizeof(char));
				state->input_buffer[state->buffer_idx] = input;
				state->buffer_idx++;

				rerender_color(state);
			}
			break;
		}
	}

	return false;
}
