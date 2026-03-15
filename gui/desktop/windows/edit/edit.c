#include "edit.h"
#include <window.h>
#include <window_helpers.h>
#include <graphics.h>
#include <desktop.h>
#include <stdlib.h>
#include <string.h>
#include <non-standard/stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <syntax.h>
#include <filepicker.h>

extern psf1_font_t font;

uint32_t color_lut[] = {
    [black]   = 0x888888,
    [red]     = 0xFF6B6B,
    [green]   = 0x88FFAA,
    [yellow]  = 0xFFFF88,
    [blue]    = 0x6B9FFF,
    [magenta] = 0xFF88FF,
    [cyan]    = 0x88FFFF,
    [white]   = 0xFFFFFF,
};



#define CHAR_W 8
#define CHAR_H 16
#define STATUS_BAR_H 16

void recompute_color(edit_state_t* st) {
    if (!st->syntax) {
        return;
    }

    if (st->color) {
        free(st->color);
    }
    st->color = highlight(st->input_buffer, st->current_size, st->syntax);
}


void move_up(edit_state_t* st) {
    if (st->buffer_ln_idx <= 0 || st->buffer_idx <= 0) {
        return;
    }

    int prev = st->buffer_idx;
    for (int i = st->buffer_idx; i > 0; i--) {
        st->buffer_idx--;
        if (st->input_buffer[i - 1] == '\n' || st->buffer_idx < 0) {
            break;
        }
    }

    if (st->buffer_idx < 0) {
        st->buffer_idx = prev;
    } else {
        st->buffer_ln_idx--;
    }
}

void move_down(edit_state_t* st) {
    if (st->buffer_ln_idx >= (int)st->ln_cnt - 1 || st->buffer_idx >= st->current_size) {
        return;
    }

    int prev = st->buffer_idx;
    for (unsigned int i = st->buffer_idx; i < st->current_size; i++) {
        st->buffer_idx++;
        if (st->input_buffer[i] == '\n' || st->buffer_idx > st->current_size) {
            break;
        }
    }

    if (st->buffer_idx > st->current_size) {
        st->buffer_idx = prev;
    } else {
        st->buffer_ln_idx++;
    }
}

void move_left(edit_state_t* st) {
    if (st->buffer_idx <= 0) {
        return;
    }

    if (st->input_buffer[st->buffer_idx - 1] == '\n') {
        st->buffer_ln_idx--;
    }

    st->buffer_idx--;
}

void move_right(edit_state_t* st) {
    if (st->buffer_idx < st->current_size) {
        if (st->input_buffer[st->buffer_idx] == '\n') {
            st->buffer_ln_idx++;
        }

        st->buffer_idx++;
    }
}


void handle_arrow(edit_state_t* st, int arrow) {
    switch (arrow) {
        case ARROW_UP:
            move_up(st);
            break;
        case ARROW_DOWN:
            move_down(st);
            break;
        case ARROW_LEFT:
            move_left(st);
            break;
        case ARROW_RIGHT:
            move_right(st);
            break;
    }
}

void handle_key(edit_state_t* st, char input) {
    if (!st->is_in_insert_mode) {
        switch (input) {
            case '\e': 
                st->is_in_insert_mode = true;
                break;
            case 'a':
                move_left(st);
                break;
            case 'd':  
                move_right(st);
                break;
            case 'w':
                move_up(st);
                break;
            case 's':
                move_down(st);
                break;
            case '+':
                st->file = freopen(st->file_name, "w", st->file);
                fseek(st->file, 0, SEEK_SET);
                fwrite(st->input_buffer, st->current_size, 1, st->file);
                fseek(st->file, st->current_size, SEEK_SET);
                ftruncate(st->file);
                st->is_edited = false;
                break;
        }
    } else {
        switch (input) {
            case '\b':
                if (st->buffer_idx <= 0 || st->current_size <= 0) {
                    break;
                }
                
                if (st->input_buffer[st->buffer_idx - 1] == '\n') {
                    st->buffer_ln_idx--;
                }
                
                if (st->buffer_idx < st->current_size) {
                    memmove(&st->input_buffer[st->buffer_idx - 1], &st->input_buffer[st->buffer_idx], (st->current_size - st->buffer_idx));
                }

                if (st->input_buffer[st->buffer_idx] == '\n') {
                    st->ln_cnt--;
                } else {
                    st->char_cnt--;
                }
                st->input_buffer = realloc(st->input_buffer, --st->current_size);
                st->buffer_idx--;
                recompute_color(st);
                break;

            case '\e':
                st->is_in_insert_mode = false;
                break;

            default:
                if (input == '\n') {
                    st->ln_cnt++; st->buffer_ln_idx++;
                } else {
                    st->char_cnt++;
                }

                st->is_edited = true;
                st->current_size++;
                st->input_buffer = realloc(st->input_buffer, st->current_size);
                memmove(&st->input_buffer[st->buffer_idx + 1], &st->input_buffer[st->buffer_idx], (st->current_size - st->buffer_idx - 1));
                st->input_buffer[st->buffer_idx] = input;
                st->buffer_idx++;
                recompute_color(st);
                break;
        }
    }
}


static char s_pending_path[128];

void edit_load_file(edit_state_t* st, const char* path);

void edit_init(window_instance_t* w) {
    edit_state_t* st = malloc(sizeof(edit_state_t));
    memset(st, 0, sizeof(edit_state_t));
    st->is_in_insert_mode = true;
    w->state = st;
    w->title_bar_color = 0x445566;

    if (s_pending_path[0]) {
        edit_load_file(st, s_pending_path);
        s_pending_path[0] = '\0';
    }
}

char* get_file_extension(const char* filename) {
	char* chr_ptr = strchr(filename, '.');
	if (chr_ptr == NULL) {
		return "";
	}
	return ++chr_ptr;
}

void edit_load_file(edit_state_t* st, const char* path) {
    int plen = strlen(path);
    if (plen >= (int)sizeof(st->file_name)) {
        plen = sizeof(st->file_name) - 1;
    }

    memcpy(st->file_name, path, plen);
    st->file_name[plen] = '\0';

    st->file = fopen(path, "r");
    if (!st->file) {
        return;
    }

    fsize(st->file, file_size);
    st->input_buffer = malloc(file_size ? file_size : 1);
    memset(st->input_buffer, 0, file_size ? file_size : 1);
    st->current_size = file_size;
    st->buffer_idx = file_size;

    if (file_size) {
        fread(st->input_buffer, file_size, 1, st->file);
    }

    st->ln_cnt = 0;
    st->char_cnt = 0;
    for (unsigned int i = 0; i < st->current_size; i++) {
        st->char_cnt++;
        if (st->input_buffer[i] == '\n') {
            st->ln_cnt++; st->char_cnt--;
        }
    }
    st->buffer_ln_idx = st->ln_cnt;

    if (!getenv("NOSYX")) {
		char syx[128] = { 0 };
		strcat(syx, getenv("ROOT_FS"));
		strcat(syx, "syntax/");
		strcat(syx, get_file_extension(st->file_name));
		strcat(syx, ".syx");
        st->syntax = load_syntax(syx);
	}

    recompute_color(st);
}

static void edit_picker_callback(const char* path) {
    edit_open(path);
}

void edit_update(window_instance_t* w, event_t* event) {
    edit_state_t* st = (edit_state_t*)w->state;
    if (!st->input_buffer) {
        return;
    }

    if (event->type == EVENT_KEY_PRESS && event->key) {
        handle_key(st, event->key);
        w->is_dirty = true;
    } else if (event->type == EVENT_ARROW_KEY && event->arrow) {
        handle_arrow(st, event->arrow);
        w->is_dirty = true;
    }
}

void edit_draw(window_instance_t* w) {
    edit_state_t* st = (edit_state_t*)w->state;

    if (!st->input_buffer) {
        window_draw_string(w, 4, 4, "No file loaded", 0xFF6B6B);
        return;
    }

    int content_h = w->height - TITLE_BAR_HEIGHT - STATUS_BAR_H;
    int viewport_rows = content_h / CHAR_H;
    if (viewport_rows < 1) viewport_rows = 1;

    char gutter_buf[16] = { 0 };
    sprintf(gutter_buf, "%d.", st->ln_cnt > 0 ? st->ln_cnt : 1);
    int gutter_chars = strlen(gutter_buf) + 1;
    int gutter_px = gutter_chars * CHAR_W;

    if ((int)st->buffer_ln_idx < st->scroll_offset) {
        st->scroll_offset = st->buffer_ln_idx;
    } else if ((int)st->buffer_ln_idx >= st->scroll_offset + viewport_rows) {
        st->scroll_offset = st->buffer_ln_idx - viewport_rows + 1;
    }
    if (st->scroll_offset < 0) {
        st->scroll_offset = 0;
    }

    int line = 0; 
    int draw_y = 0; 
    int draw_x = gutter_px; 
    bool cursor_drawn = false;

    int vis_start_drawn = 0;

    for (unsigned int i = 0; i < st->current_size; i++) {
        bool in_view = (line >= st->scroll_offset && line < st->scroll_offset + viewport_rows);

        if (in_view) {
            if (draw_y + CHAR_H > content_h) {
                break;
            }

            if (!vis_start_drawn || (draw_x == gutter_px && i > 0 && st->input_buffer[i - 1] == '\n')) {
                int display_line = line + 1;
                char ln_buf[16] = { 0 };
                sprintf(ln_buf, "%d.", display_line);
                window_draw_string(w, 0, draw_y, ln_buf, 0x888888);
                vis_start_drawn = 1;
            }

            if (i == st->buffer_idx) {
                desktop_draw_char(&font, w->x + draw_x, w->y + TITLE_BAR_HEIGHT + draw_y, '|', 0x88FFFF, 0x1a1a2e);
                draw_x += CHAR_W;
                cursor_drawn = true;
            }

            if (st->input_buffer[i] >= 0x20 && st->input_buffer[i] <= 0x7E) {
                uint32_t fg = 0xFFFFFF;
                if (st->color && i < st->current_size) {
                    fg = color_lut[st->color[i]];
                }
                desktop_draw_char(&font, w->x + draw_x, w->y + TITLE_BAR_HEIGHT + draw_y, st->input_buffer[i], fg, 0x1a1a2e);
            }
            draw_x += CHAR_W;

            if (st->input_buffer[i] == '\n') {
                line++;
                draw_x = gutter_px;
                draw_y += CHAR_H;
            } else if (draw_x + CHAR_W >= w->width) {
                draw_x = gutter_px;
                draw_y += CHAR_H;
            }
        } else {
            if (st->input_buffer[i] == '\n') line++;
        }
    }

    if (!cursor_drawn) {
        if (line >= st->scroll_offset && line < st->scroll_offset + viewport_rows && draw_y + CHAR_H <= content_h) {
            if (!vis_start_drawn) {
                char ln_buf[16] = { 0 };
                sprintf(ln_buf, "%d.", line + 1);
                window_draw_string(w, 0, draw_y, ln_buf, 0x888888);
            }
            desktop_draw_char(&font, w->x + draw_x, w->y + TITLE_BAR_HEIGHT + draw_y, '|', 0x88FFFF, 0x1a1a2e);
        }
    }

    int status_y = w->height - STATUS_BAR_H;
    for (int x = 0; x < w->width; x++) {
        for (int y = status_y; y < w->height; y++) {
            window_set_pixel(w, x, y, 0x333344);
        }
    }

    char status[256] = { 0 };
    sprintf(status, " %s [%c] %s  Ln %d/%d", st->file_name, st->is_edited ? '*' : '-', st->is_in_insert_mode ? "INSERT" : "EDIT", st->buffer_ln_idx + 1, st->ln_cnt > 0 ? st->ln_cnt : 1);

    desktop_draw_string(&font, w->x, w->y + status_y, status, 0xFFFFFF, 0x333344);
}

void edit_cleanup(window_instance_t* w) {
    edit_state_t* st = (edit_state_t*)w->state;
    if (st) {
        if (st->input_buffer) {
            free(st->input_buffer);
        }

        if (st->color) {
            free(st->color);
        }

        if (st->syntax) {
            free(st->syntax);
        }

        if (st->file) {
            fclose(st->file);
        }

        free(st);
        w->state = NULL;
    }
}

window_definition_t edit_definition = {
    .name = "Editor",
    .register_window = edit_open_picker,
};

void edit_open_picker() {
    filepicker_open(edit_picker_callback);
}

void edit_open(const char* path) {
    int plen = strnlen(path, sizeof(s_pending_path) - 1);
    memcpy(s_pending_path, path, plen);
    s_pending_path[plen] = '\0';

    const char* base = path;
    for (const char* p = path; *p; p++) {
        if (*p == '/') {
            base = p + 1;
        }
    }
    char title[64] = { 0 };
    sprintf(title, "Edit: %s", base);

    window_add(80, 60, 500, 380, title, 0x1a1a2e, edit_init, edit_update, edit_draw, edit_cleanup);
}
