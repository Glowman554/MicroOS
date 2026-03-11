#include "terminal.h"
#include "output.h"
#include "commands.h"

#include <window.h>
#include <window_helpers.h>
#include <graphics.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/env.h>

extern psf1_font_t font;

#define CHAR_W   8
#define CHAR_H   16
#define BG_COLOR 0x1a1a2e

#define TEXT_COLOR  0xCCCCCC
#define INPUT_COLOR 0xFFFFFF


void terminal_init(window_instance_t* w) {
    terminal_state_t* st = malloc(sizeof(terminal_state_t));
    memset(st, 0, sizeof(terminal_state_t));

    st->output_buf = malloc(TERM_OUTPUT_SIZE);
    memset(st->output_buf, 0, TERM_OUTPUT_SIZE);
    st->output_len = 0;

    st->input_len = 0;
    memset(st->input_buf, 0, TERM_INPUT_SIZE + 1);

    st->history = NULL;
    st->history_size = 0;
    st->history_index = 0;
    st->scroll_offset = 0;

    st->envp = (char**)env(SYS_GET_ENVP_ID);

    w->state = st;
    w->bg_color = BG_COLOR;
    w->title_bar_color = 0x16213e;

    term_print_prompt(st);
}


void history_add(terminal_state_t* st, const char* cmd) {
    if (st->history_size == 0) {
        st->history = malloc(sizeof(char*));
    } else if (st->history_size < TERM_HISTORY_MAX) {
        st->history = realloc(st->history, sizeof(char*) * (st->history_size + 1));
    } else {
        free(st->history[0]);
        memmove(st->history, st->history + 1, sizeof(char*) * (TERM_HISTORY_MAX - 1));
        st->history_size--;
    }
    st->history[st->history_size] = strdup(cmd);
    st->history_size++;
    st->history_index = st->history_size;
}

void history_recall(terminal_state_t* st) {
    if (st->history_index < 0 || st->history_index >= st->history_size) {
        return;
    }
    memset(st->input_buf, 0, TERM_INPUT_SIZE + 1);
    strcpy(st->input_buf, st->history[st->history_index]);
    st->input_len = strlen(st->input_buf);
}


void terminal_update(window_instance_t* w, event_t* event) {
    terminal_state_t* st = (terminal_state_t*)w->state;

    if (event->type == EVENT_KEY_PRESS) {
        char key = event->key;

        if (key == '\n') {
            term_append(st, st->input_buf, st->input_len);
            term_puts(st, "\n");

            if (st->input_len > 0) {
                char cmd_copy[TERM_INPUT_SIZE + 1];
                memcpy(cmd_copy, st->input_buf, st->input_len + 1);

                history_add(st, st->input_buf);

                bool keep_going = execute_command(st, cmd_copy);
                if (!keep_going) {
                    int count = window_get_count();
                    for (int i = 0; i < count; i++) {
                        if (window_get(i) == w) {
                            window_close(i);
                            return;
                        }
                    }
                    return;
                }
            }

            memset(st->input_buf, 0, TERM_INPUT_SIZE + 1);
            st->input_len = 0;
            st->scroll_offset = 0;
            term_print_prompt(st);

        } else if (key == '\b') {
            if (st->input_len > 0) {
                st->input_len--;
                st->input_buf[st->input_len] = '\0';
            }
        } else if (key == 27) {
            memset(st->input_buf, 0, TERM_INPUT_SIZE + 1);
            st->input_len = 0;
        } else if (key >= 0x20 && key <= 0x7E) {
            if (st->input_len < TERM_INPUT_SIZE) {
                st->input_buf[st->input_len++] = key;
                st->input_buf[st->input_len] = '\0';
            }
        }
        w->is_dirty = true;
    } else if (event->type == EVENT_ARROW_KEY) {
        if (event->arrow == ARROW_UP) {
            if (st->history_index > 0) {
                st->history_index--;
                history_recall(st);
            }
            w->is_dirty = true;
        } else if (event->arrow == ARROW_DOWN) {
            if (st->history_index < st->history_size - 1) {
                st->history_index++;
                history_recall(st);
            } else if (st->history_index == st->history_size - 1) {
                st->history_index = st->history_size;
                memset(st->input_buf, 0, TERM_INPUT_SIZE + 1);
                st->input_len = 0;
            }
            w->is_dirty = true;
        } else if (event->arrow == ARROW_UP + 10) {
            st->scroll_offset += 3;
            w->is_dirty = true;
        } else if (event->arrow == ARROW_DOWN + 10) {
            if (st->scroll_offset > 0) st->scroll_offset -= 3;
            w->is_dirty = true;
        }
    }
}

    #define EMIT_CHAR(ch, color_val) { \
        if (cur_line >= start_line && cur_line < start_line + rows) { \
            draw_x = (col_pos) * CHAR_W; \
            draw_y = (cur_line - start_line) * CHAR_H; \
            window_draw_char(w, draw_x, TITLE_BAR_HEIGHT + draw_y, ch, color_val, BG_COLOR); \
        } \
    }

void terminal_draw(window_instance_t* w) {
    terminal_state_t* st = (terminal_state_t*)w->state;

    int content_w = w->width;
    int content_h = w->height - TITLE_BAR_HEIGHT;
    int cols = content_w / CHAR_W;
    int rows = content_h / CHAR_H;

    if (cols <= 0 || rows <= 0) {
        return;
    }

    int total_lines = 1;
    int col = 0;

    for (int i = 0; i < st->output_len; i++) {
        if (st->output_buf[i] == '\n') {
            total_lines++;
            col = 0;
        } else {
            col++;
            if (col >= cols) {
                total_lines++;
                col = 0;
            }
        }
    }

    for (int i = 0; i < st->input_len; i++) {
        if (st->input_buf[i] == '\n') {
            total_lines++;
            col = 0;
        } else {
            col++;
            if (col >= cols) {
                total_lines++;
                col = 0;
            }
        }
    }

    int start_line = total_lines - rows + st->scroll_offset;
    if (start_line < 0) {
        start_line = 0;
    }

    int draw_x = 0;
    int draw_y = 0;



    int col_pos = 0;
    int cur_line = 0;

    for (int i = 0; i < st->output_len; i++) {
        char c = st->output_buf[i];
        if (c == '\n') {
            while (col_pos < cols) {
                EMIT_CHAR(' ', BG_COLOR);
                col_pos++;
            }
            cur_line++;
            col_pos = 0;
        } else {
            EMIT_CHAR(c, TEXT_COLOR);
            col_pos++;
            if (col_pos >= cols) {
                cur_line++;
                col_pos = 0;
            }
        }
    }

    for (int i = 0; i < st->input_len; i++) {
        char c = st->input_buf[i];
        if (c == '\n') {
            while (col_pos < cols) {
                EMIT_CHAR(' ', BG_COLOR);
                col_pos++;
            }
            cur_line++;
            col_pos = 0;
        } else {
            EMIT_CHAR(c, INPUT_COLOR);
            col_pos++;
            if (col_pos >= cols) {
                cur_line++;
                col_pos = 0;
            }
        }
    }

    if (cur_line >= start_line && cur_line < start_line + rows) {
        draw_x = col_pos * CHAR_W;
        draw_y = (cur_line - start_line) * CHAR_H;
        window_draw_rect(w, draw_x, draw_y, CHAR_W - 4, CHAR_H - 4, 0xCCCCCC);
    }

    int next_row = cur_line - start_line + 1;
    if (next_row < rows) {
        window_draw_rect(w, 0, next_row * CHAR_H, content_w, (rows - next_row) * CHAR_H, BG_COLOR);
    }
}


void terminal_cleanup(window_instance_t* w) {
    terminal_state_t* st = (terminal_state_t*)w->state;

    if (st->output_buf) {
        free(st->output_buf);
    }

    if (st->history) {
        for (int i = 0; i < st->history_size; i++) {
            free(st->history[i]);
        }
        free(st->history);
    }
    free(st);
    w->state = NULL;
}


window_definition_t terminal_definition = {
    .name = "Terminal",
    .register_window = register_terminal_window,
};

void register_terminal_window(void) {
    window_add(60, 60, 640, 400, "Terminal", BG_COLOR, terminal_init, terminal_update, terminal_draw, terminal_cleanup);
}
