#include <filepicker.h>
#include <window.h>
#include <window_helpers.h>
#include <graphics.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/file.h>

extern psf1_font_t font;
extern fpic_image_t back_button;
extern fpic_image_t up_arrow;
extern fpic_image_t down_arrow;

#define FP_ROW_H   16
#define FP_HDR_H   32

typedef struct {
    int x;
    int y;
    int width;
    int height;
} fp_btn_t;

typedef struct {
    click_area_t* rows;
    int offset;
    int max_rows;
    bool fs_mode;
    char cwd[128];
    filepicker_callback_t callback;
} filepicker_state_t;

void fp_init(window_instance_t* w) {
    filepicker_state_t* st = malloc(sizeof(filepicker_state_t));
    memset(st, 0, sizeof(filepicker_state_t));
    st->max_rows = (w->height - TITLE_BAR_HEIGHT - FP_HDR_H - 16) / FP_ROW_H;
    if (st->max_rows < 1) {
        st->max_rows = 1;
    }
    st->fs_mode = true;
    st->rows = malloc(sizeof(click_area_t) * st->max_rows);
    memset(st->rows, 0, sizeof(click_area_t) * st->max_rows);
    w->state = st;
    w->title_bar_color = 0x446688;
    w->is_dirty = true;
}

static filepicker_callback_t s_pending_callback = NULL;

void fp_update(window_instance_t* w, event_t* event) {
    filepicker_state_t* st = (filepicker_state_t*)w->state;
    if (!st) {
        return;
    }

    if (!st->callback && s_pending_callback) {
        st->callback = s_pending_callback;
        s_pending_callback = NULL;
    }

    if (event->type != EVENT_MOUSE_CLICK || event->button != MOUSE_BUTTON_LEFT) {
        return;
    }

    int rx = event->x;
    int ry = event->y;

    fp_btn_t back;
    back.x = w->width - 25 * 2;
    back.y = TITLE_BAR_HEIGHT;
    back.width = 25 * 2;
    back.height = 11 * 2;
    if (rx >= back.x && rx < back.x + back.width && ry >= back.y && ry < back.y + back.height) {
        if (st->fs_mode) {
            return;
        }
        char full[128] = { 0 };
        strcpy(full, st->cwd);
        int fl = strlen(full);
        if (fl > 0 && full[fl - 1] != '/') {
            strcat(full, "/");
        }
        strcat(full, "..");
        char buf[128] = { 0 };
        if (!resolve(full, buf)) {
            st->fs_mode = true;
            memset(st->cwd, 0, sizeof(st->cwd));
        } else {
            memset(st->cwd, 0, sizeof(st->cwd));
            strcpy(st->cwd, buf);
        }
        st->offset = 0;
        w->is_dirty = true;
        return;
    }

    fp_btn_t up;
    up.x = w->width - 16;
    up.y = TITLE_BAR_HEIGHT + FP_HDR_H;
    up.width = 16;
    up.height = 16;
    if (rx >= up.x && rx < up.x + up.width &&
        ry >= up.y && ry < up.y + up.height) {
        if (st->offset > 0) { 
            st->offset--; w->is_dirty = true;
        }
        return;
    }

    fp_btn_t dn;
    dn.x = w->width - 16;
    dn.y = w->height - 16;
    dn.width = 16;
    dn.height = 16;
    if (rx >= dn.x && rx < dn.x + dn.width && ry >= dn.y && ry < dn.y + dn.height) {
        st->offset++;
        w->is_dirty = true;
        return;
    }

    for (int i = 0; i < st->max_rows; i++) {
        click_area_t* btn = &st->rows[i];
        if (!btn->width || !btn->height) {
            continue;
        }

        if (rx < btn->x || rx >= btn->x + btn->width) {
            continue;
        }

        if (ry < btn->y || ry >= btn->y + btn->height) {
            continue;
        }

        if (st->fs_mode) {
            char out[512] = { 0 };
            if (fs_at(out, st->offset + i) && out[0]) {
                strcat(out, ":/");
                memset(st->cwd, 0, sizeof(st->cwd));
                strcpy(st->cwd, out);
                st->fs_mode = false;
                st->offset = 0;
                w->is_dirty = true;
            }
        } else {
            dir_t dir;
            dir_at(st->cwd, st->offset + i, &dir);
            if (dir.is_none) {
                break;
            }

            char full[256] = { 0 };
            strcpy(full, st->cwd);
            int fl = strlen(full);
            if (fl > 0 && full[fl - 1] != '/') {
                strcat(full, "/");
            }

            strcat(full, dir.name);

            if (dir.type == ENTRY_DIR) {
                char buf[128] = { 0 };
                if (!resolve(full, buf)) {
                    st->fs_mode = true;
                    memset(st->cwd, 0, sizeof(st->cwd));
                } else {
                    memset(st->cwd, 0, sizeof(st->cwd));
                    strcpy(st->cwd, buf);
                }
                st->offset = 0;
                w->is_dirty = true;
            } else if (dir.type == ENTRY_FILE) {
                filepicker_callback_t cb = st->callback;
                int count = window_get_count();
                for (int j = 0; j < count; j++) {
                    if (window_get(j) == w) {
                        window_close(j);
                        break;
                    }
                }
                if (cb) {
                    cb(full);
                }
                return;
            }
        }
        break;
    }
}

static void fp_draw(window_instance_t* w) {
    filepicker_state_t* st = (filepicker_state_t*)w->state;

    for (int x = 0; x < w->width; x++) {
        for (int y = TITLE_BAR_HEIGHT; y < w->height; y++) {
            window_set_pixel(w, x, y, 0x445533);
        }
    }

    window_draw_string(w, 2, 2, st->cwd[0] ? st->cwd : "(select a filesystem)", 0xffffff);
    window_draw_line(w, 0, 20, w->width, 20, 0x444444);

    window_draw_fpic_scaled(w, &back_button, w->width - 25 * 2, TITLE_BAR_HEIGHT, 2);
    window_draw_fpic(w, &up_arrow, w->width - 16, TITLE_BAR_HEIGHT + FP_HDR_H);
    window_draw_fpic(w, &down_arrow, w->width - 16, w->height - 16);

    int start_y = FP_HDR_H;

    if (st->fs_mode) {
        for (int i = 0; i < st->max_rows; i++) {
            char out[512] = { 0 };
            if (!fs_at(out, st->offset + i)) {
                break;
            }

            int y = start_y + i * FP_ROW_H;
            window_draw_string(w, 2, y, out, 0x90EE90);

            st->rows[i].x = 0;
            st->rows[i].y = TITLE_BAR_HEIGHT + y;
            st->rows[i].width = w->width - 32;
            st->rows[i].height = FP_ROW_H;
        }
    } else {
        dir_t dir;
        int cnt = 0;
        for (int i = st->offset; i < st->offset + st->max_rows && cnt < st->max_rows; i++) {
            dir_at(st->cwd, i, &dir);
            if (dir.is_none) {
                break;
            }

            int y = start_y + cnt * FP_ROW_H;
            uint32_t color = (dir.type == ENTRY_DIR) ? 0x90EE90 : 0xFF6B6B;
            window_draw_string(w, 2, y, dir.name, color);
            window_draw_char(w, w->width - 32, TITLE_BAR_HEIGHT + y, dir.type == ENTRY_FILE ? 'F' : 'D', 0xffffff, 0x1a1a2e);

            st->rows[cnt].x = 0;
            st->rows[cnt].y = TITLE_BAR_HEIGHT + y;
            st->rows[cnt].width = w->width - 32;
            st->rows[cnt].height = FP_ROW_H;
            cnt++;
        }
    }
}

static void fp_cleanup(window_instance_t* w) {
    filepicker_state_t* st = (filepicker_state_t*)w->state;
    if (st) {
        if (st->rows) free(st->rows);
        free(st);
        w->state = NULL;
    }
}


void filepicker_open(filepicker_callback_t callback) {
    s_pending_callback = callback;
    window_add(120, 80, 380, 320, "Open File", 0x445533, fp_init, fp_update, fp_draw, fp_cleanup);

    window_instance_t* w = window_get(window_get_count() - 1);
    if (w && w->state) {
        filepicker_state_t* st = (filepicker_state_t*)w->state;
        if (!st->callback) {
            st->callback = s_pending_callback;
            s_pending_callback = NULL;
        }
    }
}
