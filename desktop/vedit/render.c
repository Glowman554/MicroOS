#include <erender.h>

#include <string.h>
#include <stdlib.h>

#include <window/font.h>
#include <window.h>

#define CHAR_SIZE 16
#define CHAR_WIDTH 8
#define GRAPHICS_RENDERING_MODE
#define CURSOR_COLOR 0x33cccc
#define LINE_NUMBER_COLOR 0xa9a9a9
#define SPACE_BETWEEN_LINE_NUMBER_TEXT 1 * CHAR_WIDTH

syntax_header_t* syntax = NULL;

uint32_t color_translation[] = {
	/*
	FOREGROUND_BLACK,
	FOREGROUND_RED,
	FOREGROUND_GREEN,
	FOREGROUND_YELLOW,
	FOREGROUND_BLUE,
	FOREGROUND_MAGENTA,
	FOREGROUND_CYAN,
	FOREGROUND_WHITE
	*/
	0xFF000000, // black
    0xFFAA0000, // red
    0xFF00AA00, // green
    0xFFFFFF00, // yellow
    0xFF0000AA, // blue
    0xFFAA00AA, // magenta
    0xFF00AAAA, // cyan
    0xFFFFFFFF, // white
};

uint8_t* color = NULL;

void rerender_color(edit_state_t* state) {
	if (syntax) {	
		if (color) {
			free(color);
		}
		color = highlight(state->input_buffer, state->current_size, syntax);
	}
}

void render_ui(edit_state_t* state) {
	if (!color) {
		rerender_color(state);
	}

    window_clear(0);

    int max_length_before_line_wrap = window->window_width / CHAR_WIDTH;
	char buff[512] = { 0 };
	sprintf(buff, "File: %s [%c] Mode: --%s-- Current Line: %d Line: %d", state->file_name, state->is_edited ? '*' : '-',  state->is_in_insert_mode ? "INSERT" : "EDIT", state->buffer_ln_idx, state->ln_cnt);

	draw_string_window(&font, 0, window->window_height - CHAR_SIZE, buff, 0xffffffff, 0);

	int j = 0;
	int cur_x;
	int cur_y = 0;
	int already_drawn  = 0;
	bool initial_line_drawn = false;
	int current_line = 1;
	bool cursor_drawn = false;

	sprintf(buff, "%d .", state->ln_cnt);
	int space_to_draw = SPACE_BETWEEN_LINE_NUMBER_TEXT + (strlen(buff) * CHAR_WIDTH);
	cur_x = space_to_draw;

	int possible_lines_to_draw = window->window_height / CHAR_SIZE - 4;

	draw_line_window(space_to_draw - (1 * CHAR_WIDTH), 0, space_to_draw - (1 * CHAR_WIDTH), window->window_height - (2 * CHAR_WIDTH), LINE_NUMBER_COLOR);

	for (int i = 0; i < state->current_size; i++) {
		if ((state->ln_cnt - 1 < possible_lines_to_draw || j >= state->buffer_ln_idx) && already_drawn <= possible_lines_to_draw) {
			if (!initial_line_drawn) {
				initial_line_drawn = true;
				sprintf(buff, "%d.", current_line);
				draw_string_window(&font, 0, cur_y, buff, LINE_NUMBER_COLOR, 0);
			}

			if (i == state->buffer_idx) {
				draw_char_window(&font, cur_x, cur_y, '|', CURSOR_COLOR, 0);
				cur_x += CHAR_WIDTH;
				cursor_drawn = true;
			}

			if (state->input_buffer[i] >= 0x20 && state->input_buffer[i] <= 0x7E) {
				draw_char_window(&font, cur_x, cur_y, state->input_buffer[i], color ? color_translation[color[i]] : 0xffffffff, 0);
			}

			cur_x += CHAR_WIDTH;

			if (state->input_buffer[i] == '\n') {
				already_drawn++;
				current_line++;
				cur_x = space_to_draw;
				cur_y += CHAR_SIZE;
				sprintf(buff, "%d.", current_line);
				draw_string_window(&font, 0, cur_y, buff, LINE_NUMBER_COLOR, 0);
			} else if ((cur_x / CHAR_WIDTH) % max_length_before_line_wrap == 0) {
                cur_y += CHAR_SIZE;
				cur_x = space_to_draw;
				already_drawn++;
            }
		} else {
			if (state->input_buffer[i] == '\n') {
				current_line++;
				j++;
			}
		}
	}

	if (!cursor_drawn) {
		if (!initial_line_drawn) {
			sprintf(buff, "%d.", current_line);
			draw_string_window(&font, 0, cur_y, buff, LINE_NUMBER_COLOR, 0);
		}
		draw_char_window(&font, cur_x, cur_y, '|', CURSOR_COLOR, 0);
	}
}