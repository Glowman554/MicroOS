#include "explorer.h"
#include <window.h>
#include <window_helpers.h>
#include <graphics.h>
#include <desktop.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/file.h>

extern psf1_font_t font;
extern fpic_image_t back_button;
extern fpic_image_t up_arrow;
extern fpic_image_t down_arrow;

void format_size(char* out, size_t size) {
    if (size < 1024) {
        sprintf(out, "%d b", size);
    } else if (size < 1024 * 1024) {
        sprintf(out, "%d Kb", size / 1024);
    } else if (size < 1024 * 1024 * 1024) {
        sprintf(out, "%d Mb", size / (1024 * 1024));
    } else {
        sprintf(out, "%d Gb", size / (1024 * 1024 * 1024));
    }
}

typedef struct {
    int x;
    int y;
    int width;
    int height;
} button_area_t;

void explorer_init(window_instance_t* w) {
    explorer_state_t* state = malloc(sizeof(explorer_state_t));
    state->offset = 0;
    state->max_rows = (w->height - TITLE_BAR_HEIGHT - 50) / 16;
    if (state->max_rows < 1) {
        state->max_rows = 1;
    }
    state->fs_mode = true;
    state->files = malloc(sizeof(click_area_t) * state->max_rows);
    memset(state->files, 0, sizeof(click_area_t) * state->max_rows);
    memset(state->cwd, 0, 64);
    w->state = state;
    w->title_bar_color = 0x4488ff;
}

void explorer_update(window_instance_t* w, event_t* event) {
    explorer_state_t* state = (explorer_state_t*)w->state;
    
    if (event->type == EVENT_MOUSE_CLICK && event->button == MOUSE_BUTTON_LEFT) {
        int rel_x = event->x;
        int rel_y = event->y;
        
        button_area_t back_button_area;
        back_button_area.x = w->width - 25 * 2;
        back_button_area.y = TITLE_BAR_HEIGHT;
        back_button_area.width = 25 * 2;
        back_button_area.height = 11 * 2;
        
        if (rel_x >= back_button_area.x && rel_x < back_button_area.x + back_button_area.width &&
            rel_y >= back_button_area.y && rel_y < back_button_area.y + back_button_area.height) {
            char full_path[128] = { 0 };
            strcpy(full_path, state->cwd);
            int fp_len = strlen(full_path);
            if (fp_len > 0 && full_path[fp_len - 1] != '/') {
                strcat(full_path, "/");
            }
            strcat(full_path, "..");
            char path_buf[64] = { 0 };
            if (!resolve(full_path, path_buf)) {
                state->fs_mode = true;
                memset(state->cwd, 0, 64);
            } else {
                memset(state->cwd, 0, 64);
                strcpy(state->cwd, path_buf);
            }
            state->offset = 0;
            w->is_dirty = true;
            return;
        }
        
        button_area_t up_arrow_area;
        up_arrow_area.x = w->width - 16;
        up_arrow_area.y = TITLE_BAR_HEIGHT + 32;
        up_arrow_area.width = 16;
        up_arrow_area.height = 16;
        
        if (rel_x >= up_arrow_area.x && rel_x < up_arrow_area.x + up_arrow_area.width &&
            rel_y >= up_arrow_area.y && rel_y < up_arrow_area.y + up_arrow_area.height) {
            if (state->offset > 0) {
                state->offset--;
                w->is_dirty = true;
            }
            return;
        }
        
        button_area_t down_arrow_area;
        down_arrow_area.x = w->width - 16;
        down_arrow_area.y = w->height - 16;
        down_arrow_area.width = 16;
        down_arrow_area.height = 16;
        
        if (rel_x >= down_arrow_area.x && rel_x < down_arrow_area.x + down_arrow_area.width &&
            rel_y >= down_arrow_area.y && rel_y < down_arrow_area.y + down_arrow_area.height) {
            state->offset++;
            w->is_dirty = true;
            return;
        }
        
        for (int i = 0; i < state->max_rows; i++) {
            click_area_t* btn = &state->files[i];
            if (btn->width && btn->height &&
                rel_x >= btn->x && rel_x < btn->x + btn->width &&
                rel_y >= btn->y && rel_y < btn->y + btn->height) {
                
                if (state->fs_mode) {
                    char out[512] = { 0 };
                    fs_at(out, state->offset + i);
                    if (out[0]) {
                        strcat(out, ":/");
                        memset(state->cwd, 0, 64);
                        strcpy(state->cwd, out);
                        state->fs_mode = false;
                        state->offset = 0;
                        w->is_dirty = true;
                    }
                } else {
                    dir_t dir;
                    dir_at(state->cwd, state->offset + i, &dir);
                    
                    if (!dir.is_none) {
                        if (dir.type == ENTRY_DIR) {
                            char full_path[128] = { 0 };
                            strcpy(full_path, state->cwd);
                            int fp_len = strlen(full_path);
                            if (fp_len > 0 && full_path[fp_len - 1] != '/') {
                                strcat(full_path, "/");
                            }
                            strcat(full_path, dir.name);
                            char path_buf[64] = { 0 };
                            if (!resolve(full_path, path_buf)) {
                                state->fs_mode = true;
                                memset(state->cwd, 0, 64);
                                state->offset = 0;
                            } else {
                                memset(state->cwd, 0, 64);
                                strcpy(state->cwd, path_buf);
                                state->offset = 0;
                            }
                            w->is_dirty = true;
                        } else if (dir.type == ENTRY_FILE) {
                            char full_path[128] = { 0 };
                            strcpy(full_path, state->cwd);
                            int fp_len = strlen(full_path);
                            if (fp_len > 0 && full_path[fp_len - 1] != '/') {
                                strcat(full_path, "/");
                            }
                            strcat(full_path, dir.name);
                            desktop_open_file(full_path);
                        }
                    }
                }
                break;
            }
        }
    }
}

void explorer_draw(window_instance_t* w) {
    explorer_state_t* state = (explorer_state_t*)w->state;
    
    for (int x = 0; x < w->width; x++) {
        for (int y = TITLE_BAR_HEIGHT; y < w->height; y++) {
            desktop_set_pixel(w->x + x, w->y + y, 0x1a1a2e);
        }
    }
    
    window_draw_string(w, 2, 2, state->cwd, 0xffffff);
    
    window_draw_line(w, 0, 20, w->width, 20, 0x444444);
    
    button_area_t back_button_area;
    back_button_area.x = w->width - 25 * 2;
    back_button_area.y = TITLE_BAR_HEIGHT;
    back_button_area.width = 25 * 2;
    back_button_area.height = 11 * 2;
    desktop_draw_fpic_scaled(&back_button, w->x + back_button_area.x, w->y + back_button_area.y, 2);
    
    button_area_t up_arrow_area;
    up_arrow_area.x = w->width - 16;
    up_arrow_area.y = TITLE_BAR_HEIGHT + 32;
    up_arrow_area.width = 16;
    up_arrow_area.height = 16;
    desktop_draw_fpic(&up_arrow, w->x + up_arrow_area.x, w->y + up_arrow_area.y);
    
    button_area_t down_arrow_area;
    down_arrow_area.x = w->width - 16;
    down_arrow_area.y = w->height - 16;
    down_arrow_area.width = 16;
    down_arrow_area.height = 16;
    desktop_draw_fpic(&down_arrow, w->x + down_arrow_area.x, w->y + down_arrow_area.y);
    
    int start_y = 32;
    
    if (state->fs_mode) {
        for (int i = 0; i < state->max_rows; i++) {
            char out[512] = { 0 };
            if (!fs_at(out, state->offset + i)) {
                break;
            }
            
            int y = start_y + i * 16;
            window_draw_string(w, 2, y, out, 0x90EE90);
            
            state->files[i].x = 0;
            state->files[i].y = TITLE_BAR_HEIGHT + y;
            state->files[i].width = w->width - 32;
            state->files[i].height = 16;
            
        }
    } else {
        dir_t dir;
        int count = 0;
        for (int i = state->offset; i < state->offset + state->max_rows && count < state->max_rows; i++) {
            dir_at(state->cwd, i, &dir);
            if (dir.is_none) {
                break;
            }
            
            int y = start_y + count * 16;
            uint32_t color = (dir.type == ENTRY_DIR) ? 0x90EE90 : 0xFF6B6B;
            window_draw_string(w, 2, y, dir.name, color);
            desktop_draw_char(&font, w->x + w->width - 32, w->y + TITLE_BAR_HEIGHT + y, dir.type == ENTRY_FILE ? 'F' : 'D', 0xffffff, 0x1a1a2e);
            
            state->files[count].x = 0;
            state->files[count].y = TITLE_BAR_HEIGHT + y;
            state->files[count].width = w->width - 32;
            state->files[count].height = 16;
            
            count++;
        }
    }
}

void explorer_cleanup(window_instance_t* w) {
    explorer_state_t* state = (explorer_state_t*)w->state;
    if (state) {
        if (state->files) {
            free(state->files);
        }
        free(state);
        w->state = NULL;
    }
}

window_definition_t explorer_definition = {
    .name = "Explorer",
    .register_window = register_explorer_window,
};

void register_explorer_window(void) {
    window_add(100, 100, 400, 300, "Explorer", 0x1a1a2e, explorer_init, explorer_update, explorer_draw, explorer_cleanup);
}
