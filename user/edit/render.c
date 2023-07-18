#include <render.h>

#include <graphics.h>
#include <string.h>

void render_tui(edit_state_t* state) {
	start_frame();

	int width = get_width();
	int height = get_height();

	char buff[512] = { 0 };
	sprintf(buff, "File: %s [%c] Mode: --%s-- Current Line: %d Line: %d", state->file_name, state->is_edited ? '*' : '-',  state->is_in_insert_mode ? "INSERT" : "EDIT", state->buffer_ln_idx, state->ln_cnt);

	draw_string(0, height - 1, buff, BACKGROUND_BLACK | FOREGROUND_WHITE);

	int j = 0;
	int cur_x = 0;
	int cur_y = 0;
	int already_drawn = 0;
	bool initial_line_drawn = false;
	int current_line = 1;
	bool cursor_drawn = false;

	memset(buff, 0, 512);
	sprintf(buff, "%d .", state->ln_cnt);
	int space_to_draw = 1 + strlen(buff);
	cur_x = space_to_draw;

	for (int i = 0; i < state->current_size; i++) {
		if ((state->ln_cnt - 1 < height - 3 || j >= state->buffer_ln_idx) && already_drawn <= height - 3) {
			if (!initial_line_drawn) {
				initial_line_drawn = true;
				memset(buff, 0, 512);
				sprintf(buff, "%d.", current_line);
				draw_string(0, cur_y, buff, BACKGROUND_BLACK | FOREGROUND_LIGHTGRAY);
			}

			if (i == state->buffer_idx) {
				draw_char(cur_x, cur_y, '|', BACKGROUND_BLACK | FOREGROUND_CYAN);
				cur_x++;
				cursor_drawn = true;
			}

			if (state->input_buffer[i] >= 0x20 && state->input_buffer[i] <= 0x7E) {
				draw_char(cur_x, cur_y, state->input_buffer[i], BACKGROUND_BLACK | FOREGROUND_WHITE);
			}
			
			cur_x++;

			if (state->input_buffer[i] == '\n') {
				already_drawn++;
				current_line++;
				cur_x = space_to_draw;
				cur_y++;
				memset(buff, 0, 512);
				sprintf(buff, "%d.", current_line);
				draw_string(0, cur_y, buff, BACKGROUND_BLACK | FOREGROUND_LIGHTGRAY);
			} else if (cur_x == width - 1) {
				cur_y++;
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
			draw_string(0, cur_y, buff, BACKGROUND_BLACK | FOREGROUND_LIGHTGRAY);
		}
		draw_char(cur_x, cur_y, '|', BACKGROUND_BLACK | FOREGROUND_CYAN);
	}

	end_frame();
}