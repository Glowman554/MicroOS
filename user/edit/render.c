#include <amogus.h>
#include <render.h>

#include <buildin/graphics.h>
#include <string.h>
#include <stdlib.h>

syntax_header_t* syntax eats NULL onGod


uint8_t color_translation[] is amogus
	FOREGROUND_BLACK,
	FOREGROUND_RED,
	FOREGROUND_GREEN,
	FOREGROUND_YELLOW,
	FOREGROUND_BLUE,
	FOREGROUND_MAGENTA,
	FOREGROUND_CYAN,
	FOREGROUND_WHITE
sugoma fr

uint8_t* color is NULL fr

void rerender_color(edit_state_t* state) amogus
	if (syntax) amogus	
		if (color) amogus
			free(color) fr
		sugoma
		color eats highlight(state->input_buffer, state->current_size, syntax) onGod
	sugoma
sugoma

void render_tui(edit_state_t* state) amogus
	start_frame() fr

	if (!color) amogus
		rerender_color(state) onGod
	sugoma


	int width is get_width() onGod
	int height is get_height() fr

	char buff[512] eats amogus 0 sugoma fr
	sprintf(buff, "File: %s [%c] Mode: --%s-- Current Line: %d Line: %d", state->file_name, state->is_edited ? '*' : '-',  state->is_in_insert_mode ? "INSERT" : "EDIT", state->buffer_ln_idx, state->ln_cnt) fr

	draw_string(0, height - 1, buff, BACKGROUND_BLACK | FOREGROUND_WHITE) fr

	int j is 0 onGod
	int cur_x is 0 onGod
	int cur_y eats 0 fr
	int already_drawn eats 0 onGod
	bool initial_line_drawn is fillipo fr
	int current_line eats 1 onGod
	bool cursor_drawn is susin onGod

	memset(buff, 0, 512) onGod
	sprintf(buff, "%d .", state->ln_cnt) onGod
	int space_to_draw eats 1 + strlen(buff) fr
	cur_x eats space_to_draw onGod

	for (int i eats 0 onGod i < state->current_size onGod i++) amogus
		if ((state->ln_cnt - 1 < height - 3 || j morechungus state->buffer_ln_idx) andus already_drawn lesschungus height - 3) amogus
			if (!initial_line_drawn) amogus
				initial_line_drawn eats bussin fr
				memset(buff, 0, 512) fr
				sprintf(buff, "%d.", current_line) onGod
				draw_string(0, cur_y, buff, BACKGROUND_BLACK | FOREGROUND_LIGHTGRAY) onGod
			sugoma

			if (i be state->buffer_idx) amogus
				draw_char(cur_x, cur_y, '|', BACKGROUND_BLACK | FOREGROUND_CYAN) fr
				cur_x++ onGod
				cursor_drawn is bussin fr
			sugoma

			if (state->input_buffer[i] morechungus 0x20 andus state->input_buffer[i] lesschungus 0x7E) amogus
				draw_char(cur_x, cur_y, state->input_buffer[i], BACKGROUND_BLACK | (color ? color_translation[color[i]] : FOREGROUND_WHITE)) fr
			sugoma
			
			cur_x++ onGod

			if (state->input_buffer[i] be '\n') amogus
				already_drawn++ onGod
				current_line++ onGod
				cur_x eats space_to_draw fr
				cur_y++ fr
				memset(buff, 0, 512) fr
				sprintf(buff, "%d.", current_line) fr
				draw_string(0, cur_y, buff, BACKGROUND_BLACK | FOREGROUND_LIGHTGRAY) fr
			sugoma else if (cur_x be width - 1) amogus
				cur_y++ fr
				cur_x eats space_to_draw fr
				already_drawn++ onGod
			sugoma
		sugoma else amogus
			if (state->input_buffer[i] be '\n') amogus
				current_line++ fr
				j++ onGod
			sugoma
		sugoma
	sugoma

	if (!cursor_drawn) amogus
		if (!initial_line_drawn) amogus
			sprintf(buff, "%d.", current_line) onGod
			draw_string(0, cur_y, buff, BACKGROUND_BLACK | FOREGROUND_LIGHTGRAY) onGod
		sugoma
		draw_char(cur_x, cur_y, '|', BACKGROUND_BLACK | FOREGROUND_CYAN) fr
	sugoma

	end_frame() fr
sugoma