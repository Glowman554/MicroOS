#include <terminal.h>
#include <output.h>
#include <commands.h>

#include <wm_client.h>
#include <wm_protocol.h>
#include <non-standard/sys/spawn.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BG_COLOR     0x1a1a2e
#define TEXT_COLOR   0xCCCCCC
#define INPUT_COLOR  0xFFFFFF
#define CURSOR_COLOR 0xCCCCCC

#define CHAR_W 8
#define CHAR_H 16

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

void terminal_draw(wm_client_t* client, terminal_state_t* st) {
    int content_w = wm_client_width(client);
    int content_h = wm_client_height(client);
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


    wm_client_fill_rect(client, 0, 0, content_w, content_h, BG_COLOR);

    int col_pos = 0;
    int cur_line = 0;

    #define EMIT_CHAR(ch, color_val) do { \
        if (cur_line >= start_line && cur_line < start_line + rows) { \
            int dx = col_pos * CHAR_W; \
            int dy = (cur_line - start_line) * CHAR_H; \
            wm_client_draw_char(client, dx, dy, ch, color_val, BG_COLOR); \
        } \
    } while (0)

    for (int i = 0; i < st->output_len; i++) {
        char c = st->output_buf[i];
        if (c == '\n') {
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

    #undef EMIT_CHAR

    if (cur_line >= start_line && cur_line < start_line + rows) {
        int cx = col_pos * CHAR_W;
        int cy = (cur_line - start_line) * CHAR_H;
        wm_client_fill_rect(client, cx, cy, CHAR_W - 4, CHAR_H - 4, CURSOR_COLOR);
    }
}

int main(int argc, char** argv, char** envp) {
    wm_client_t client;
    wm_client_init(&client);

    wm_client_set_title(&client, "Terminal");
    wm_client_set_title_bar_color(&client, 0x16213e);
    wm_client_set_bg_color(&client, BG_COLOR);

    terminal_state_t st;
    memset(&st, 0, sizeof(st));
    st.output_buf = malloc(TERM_OUTPUT_SIZE);
    memset(st.output_buf, 0, TERM_OUTPUT_SIZE);
    st.output_len = 0;
    st.input_len = 0;
    st.history = NULL;
    st.history_size = 0;
    st.history_index = 0;
    st.scroll_offset = 0;
    st.envp = envp;

    term_print_prompt(&st);

    terminal_draw(&client, &st);
    wm_client_flush(&client);

    while (!wm_client_should_close(&client)) {
        wm_event_t evt;
        int dirty = 0;

        while (wm_client_poll_event(&client, &evt)) {
            if (evt.type == WM_EVENT_KEY_PRESS) {
                char key = evt.key;

                if (key == '\n') {
                    term_append(&st, st.input_buf, st.input_len);
                    term_puts(&st, "\n");

                    if (st.input_len > 0) {
                        char cmd_copy[TERM_INPUT_SIZE + 1];
                        memcpy(cmd_copy, st.input_buf, st.input_len + 1);
                        history_add(&st, st.input_buf);

                        bool keep_going = execute_command(&st, cmd_copy);
                        if (!keep_going) {
                            goto done;
                        }
                    }

                    memset(st.input_buf, 0, TERM_INPUT_SIZE + 1);
                    st.input_len = 0;
                    st.scroll_offset = 0;
                    term_print_prompt(&st);
                    dirty = 1;

                } else if (key == '\b') {
                    if (st.input_len > 0) {
                        st.input_len--;
                        st.input_buf[st.input_len] = '\0';
                        dirty = 1;
                    }
                } else if (key == 27) {
                    memset(st.input_buf, 0, TERM_INPUT_SIZE + 1);
                    st.input_len = 0;
                    dirty = 1;
                } else if (key >= 0x20 && key <= 0x7E) {
                    if (st.input_len < TERM_INPUT_SIZE) {
                        st.input_buf[st.input_len++] = key;
                        st.input_buf[st.input_len] = '\0';
                        dirty = 1;
                    }
                }
            } else if (evt.type == WM_EVENT_ARROW_KEY) {
                if (evt.arrow == WM_ARROW_UP) {
                    if (st.history_index > 0) {
                        st.history_index--;
                        history_recall(&st);
                        dirty = 1;
                    }
                } else if (evt.arrow == WM_ARROW_DOWN) {
                    if (st.history_index < st.history_size - 1) {
                        st.history_index++;
                        history_recall(&st);
                    } else if (st.history_index == st.history_size - 1) {
                        st.history_index = st.history_size;
                        memset(st.input_buf, 0, TERM_INPUT_SIZE + 1);
                        st.input_len = 0;
                    }
                    dirty = 1;
                }
            }
        }

        if (dirty) {
            terminal_draw(&client, &st);
            wm_client_flush(&client);
        }

        yield();
    }

done:
    if (st.output_buf) {
        free(st.output_buf);
    }

    if (st.history) {
        for (int i = 0; i < st.history_size; i++) {
            free(st.history[i]);
        }
        
        free(st.history);
    }
    return 0;
}
