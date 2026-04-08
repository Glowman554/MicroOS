#include <render.h>

#include <string.h>
#include <stdlib.h>

#define TAB_WIDTH 4
#define CHAR_W 8
#define CHAR_H 16

#define BG_COLOR      0x1a1a2e
#define TEXT_COLOR    0xCCCCCC
#define LINENO_COLOR  0x888888
#define CURSOR_COLOR  0x00CCCC
#define STATUS_COLOR  0xFFFFFF

syntax_header_t* syntax = NULL;

uint32_t color_translation[] = {
	0x000000,
	0xCC0000,
	0x00CC00,
	0xCCCC00,
	0x0000CC,
	0xCC00CC,
	0x00CCCC,
	0xCCCCCC
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

void draw_line_number(wm_client_t* client, int cur_y, int current_line) {
	char buff[16] = { 0 };
	memset(buff, 0, sizeof(buff));
	sprintf(buff, "%d.", current_line);
	wm_client_draw_string(client, 0, cur_y * CHAR_H, buff, LINENO_COLOR, BG_COLOR);
}


void render_ui(wm_client_t* client, edit_state_t* state) {
	if (!color) {
		rerender_color(state);
	}

	int width = wm_client_width(client) / CHAR_W;
	int height = wm_client_height(client) / CHAR_H;

	wm_client_fill_rect(client, 0, 0, wm_client_width(client), wm_client_height(client), BG_COLOR);

	char buff[512] = { 0 };
	if (state->read_only) {
		sprintf(buff, "File: %s Current Line: %d Line: %d", state->file_name, state->buffer_ln_idx + 1, state->ln_cnt);
	} else {
		sprintf(buff, "File: %s [%c] -- %s -- Current Line: %d Line: %d", state->file_name, state->is_edited ? '*' : '-',  state->is_in_insert_mode ? "INSERT" : "EDIT", state->buffer_ln_idx + 1, state->ln_cnt);
	}

	wm_client_draw_string(client, 0, (height - 1) * CHAR_H, buff, STATUS_COLOR, BG_COLOR);

	int j = 0;
	int cur_x = 0;
	int cur_y = 0;
	int already_drawn = 0;
	bool initial_line_drawn = false;
	bool cursor_drawn = false;

	memset(buff, 0, 512);
	sprintf(buff, "%d .", state->ln_cnt);
	int space_to_draw = 1 + strlen(buff);
	cur_x = space_to_draw;

	int viewport_height = height - 3;
	static int start_line = 0;
	
	if (state->buffer_ln_idx < start_line) {
		start_line = state->buffer_ln_idx;
	} else if (state->buffer_ln_idx >= start_line + viewport_height) {
		start_line = state->buffer_ln_idx - viewport_height + 1;
	}
	
	if (start_line < 0) {
		start_line = 0;
	}
	if (start_line + viewport_height > state->ln_cnt) {
		start_line = state->ln_cnt - viewport_height;
		if (start_line < 0) {
			start_line = 0;
		}
	}

	int current_line = start_line + 1;

	for (int i = 0; i < state->current_size; i++) {
		if ((j >= start_line && j < start_line + viewport_height) && already_drawn <= viewport_height) {
			if (!initial_line_drawn) {
				initial_line_drawn = true;
				draw_line_number(client, cur_y, current_line);
			}

			if (i == state->buffer_idx) {
				wm_client_draw_char(client, cur_x * CHAR_W, cur_y * CHAR_H, '|', CURSOR_COLOR, BG_COLOR);
				cur_x++;
				cursor_drawn = true;
			}

			if (state->input_buffer[i] == '\t') {
				int col = cur_x - space_to_draw;
				int next_tab_stop = ((col / TAB_WIDTH) + 1) * TAB_WIDTH;
				int spaces = next_tab_stop - col;
				
				for (int s = 0; s < spaces; s++) {
					if (cur_x >= width) {
						cur_y++;
						cur_x = space_to_draw;
						already_drawn++;
						draw_line_number(client, cur_y, current_line);
					}
					wm_client_draw_char(client, cur_x * CHAR_W, cur_y * CHAR_H, state->show_tab_char ? '.' : ' ', LINENO_COLOR, BG_COLOR);
					cur_x++;
				}
				cur_x--;
			}

			if (state->input_buffer[i] >= 0x20 && state->input_buffer[i] <= 0x7E) {
				wm_client_draw_char(client, cur_x * CHAR_W, cur_y * CHAR_H, state->input_buffer[i], color ? color_translation[color[i]] : TEXT_COLOR, BG_COLOR);
			}
			
			cur_x++;

			if (state->input_buffer[i] == '\n') {
				already_drawn++;
				current_line++;
				j++;
				cur_x = space_to_draw;
				cur_y++;
				draw_line_number(client, cur_y, current_line);
			} else if (cur_x == width - 1) {
				cur_y++;
				cur_x = space_to_draw;
				already_drawn++;
			}
		} else {
			if (state->input_buffer[i] == '\n') {
				j++;
			}
		}
	}

	if (!cursor_drawn) {
		if (!initial_line_drawn) {
			sprintf(buff, "%d.", current_line);
			wm_client_draw_string(client, 0, cur_y * CHAR_H, buff, LINENO_COLOR, BG_COLOR);
		}
		wm_client_draw_char(client, cur_x * CHAR_W, cur_y * CHAR_H, '|', CURSOR_COLOR, BG_COLOR);
	}
}
