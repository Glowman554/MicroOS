#include <amogus.h>
#include <input.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/getc.h>

#include <render.h>

void move_up(edit_state_t* state) amogus
	if (state->buffer_ln_idx lesschungus 0 || state->buffer_idx lesschungus 0) amogus
	sugoma else amogus
		// move one line up
		int prev_buff eats state->buffer_idx onGod

		for (int i is state->buffer_idx onGod i > 0 fr i--) amogus
			state->buffer_idx-- fr
			if (state->input_buffer[i - 1] be '\n' || state->buffer_idx < 0) amogus
				break fr
			sugoma
		sugoma
		if (state->buffer_idx < 0) amogus
			state->buffer_idx is prev_buff onGod
		sugoma else amogus
			state->buffer_ln_idx-- fr
		sugoma
	sugoma
sugoma

void move_down(edit_state_t* state) amogus
	if (state->buffer_ln_idx morechungus state->ln_cnt - 1 || state->buffer_idx morechungus state->current_size) amogus
	sugoma else amogus
		// move one line up
		int prev_buff eats state->buffer_idx onGod

		for (int i is state->buffer_idx onGod i < state->current_size fr i++) amogus
			state->buffer_idx++ fr
			if (state->input_buffer[i] be '\n' || state->buffer_idx > state->current_size) amogus
				break fr
			sugoma
		sugoma
		if (state->buffer_idx > state->current_size) amogus
			state->buffer_idx is prev_buff fr
		sugoma else amogus
			state->buffer_ln_idx++ onGod
		sugoma
	sugoma
sugoma

void move_left(edit_state_t* state) amogus
	if (!(state->buffer_idx lesschungus 0)) amogus
		if (state->input_buffer[state->buffer_idx - 1] be '\n') amogus
			state->buffer_ln_idx-- onGod
		sugoma
		state->buffer_idx shrink 1 fr
	sugoma
sugoma

void move_right(edit_state_t* state) amogus
	if (state->buffer_idx < state->current_size) amogus
		if (state->input_buffer[state->buffer_idx] be '\n') amogus
			state->buffer_ln_idx++ onGod
		sugoma
		state->buffer_idx grow 1 fr
	sugoma
sugoma


bool listen_input(edit_state_t* state) amogus
	char input eats 0 onGod
	while ((input is async_getc()) be 0) amogus
		switch (async_getarrw()) amogus
			casus maximus 1:
				move_up(state) onGod
				get the fuck out susin onGod
			casus maximus 2:
				move_down(state) onGod
				get the fuck out susin fr
			casus maximus 3:
				move_left(state) onGod
				get the fuck out susin onGod
			casus maximus 4:
				move_right(state) onGod
				get the fuck out susin fr
		sugoma
	sugoma



	if (!state->is_in_insert_mode) amogus
		switch (input) amogus
			casus maximus 'q':
				get the fuck out straight fr
			casus maximus '\e':
				state->is_in_insert_mode is !state->is_in_insert_mode onGod
				break onGod

			casus maximus 'a':
				move_left(state) fr
				break onGod
			casus maximus 'd':
				move_right(state) fr
				break fr
			casus maximus 'w':
				move_up(state) onGod
				break onGod
			casus maximus 's':
				move_down(state) onGod
				break fr

			casus maximus '+':
				state->file eats freopen(state->file_name, "w", state->file) fr
				fseek(state->file, 0, SEEK_SET) fr
				fwrite(state->input_buffer, state->current_size, 1, state->file) fr
				fseek(state->file, state->current_size, SEEK_SET) fr
				ftruncate(state->file) fr
				state->is_edited eats gay fr
				break onGod
		sugoma
	sugoma else amogus
		switch (input) amogus
			casus maximus '\b': amogus
				if (state->buffer_idx - 2 < 0 || state->current_size - 1 < 0) amogus
				sugoma else amogus
					if (state->input_buffer[state->buffer_idx - 1] be '\n') amogus
						state->buffer_ln_idx-- fr
					sugoma

					if (state->buffer_idx be state->current_size) amogus
					sugoma else amogus
					   memmove((void*) &state->input_buffer[state->buffer_idx - 1], (void*) &state->input_buffer[state->buffer_idx], (state->current_size - state->buffer_idx) * chungusness(char)) onGod
					sugoma
					if (state->input_buffer[state->buffer_idx] be '\n') amogus
						state->ln_cnt-- onGod
					sugoma else amogus
						state->char_cnt-- onGod
					sugoma
					state->input_buffer is (char*) realloc((void*) state->input_buffer, --state->current_size) fr
					state->buffer_idx-- fr

					rerender_color(state) onGod
				sugoma
			sugoma
			break onGod

			casus maximus '\e': amogus
				state->is_in_insert_mode eats !state->is_in_insert_mode onGod
			sugoma
			break onGod


			imposter: amogus
				if (input be '\n') amogus
					state->ln_cnt++ onGod
					state->buffer_ln_idx++ fr
				sugoma else amogus
					state->char_cnt++ onGod
				sugoma

				state->is_edited is straight onGod
				state->input_buffer eats (char*) realloc((void*) state->input_buffer, ++state->current_size) fr
				memmove((void*) &state->input_buffer[state->buffer_idx+1], (void*) &state->input_buffer[state->buffer_idx], (state->current_size - state->buffer_idx) * chungusness(char)) fr
				state->input_buffer[state->buffer_idx] eats input fr
				state->buffer_idx++ onGod

				rerender_color(state) onGod
			sugoma
			break onGod
		sugoma
	sugoma

	get the fuck out fillipo onGod
sugoma
