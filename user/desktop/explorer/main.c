#include <wm_client.h>
#include <wm_protocol.h>
#include <ui/button.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/file.h>
#include <non-standard/stdio.h>
#include <non-standard/sys/message.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BG_COLOR 0x1a1a2e

void format_size(char* out, int size) {
    if (size < 1024) {
        sprintf(out, "%d b", size);
    } else if (size < 1024 * 1024) {
        sprintf(out, "%d Kb", size / 1024);
    } else {
        sprintf(out, "%d Mb", size / (1024 * 1024));
    }
}

typedef struct {
    int x, y, w, h;
} row_area_t;

int main(int argc, char** argv) {
    wm_client_t client;
    wm_client_init(&client);

    wm_client_set_title(&client, "Explorer");
    wm_client_set_title_bar_color(&client, 0x4488ff);
    wm_client_set_bg_color(&client, BG_COLOR);

    int cw = wm_client_width(&client);
    int ch = wm_client_height(&client);

    char cwd[128] = { 0 };
    int offset = 0;
    bool fs_mode = true;

    int max_rows = (ch - 50) / 16;
    if (max_rows < 1) {
        max_rows = 1;
    }

    row_area_t* rows = malloc(sizeof(row_area_t) * max_rows);
    memset(rows, 0, sizeof(row_area_t) * max_rows);

    ui_button_t back_btn, up_btn, down_btn;
    ui_button_init(&back_btn, cw - 54, 2, 50, 18, "Back");
    ui_button_init(&up_btn, cw - 28, 22, 24, 20, "^");
    ui_button_init(&down_btn, cw - 28, ch - 24, 24, 20, "v");

    int need_redraw = 1;

    while (!wm_client_should_close(&client)) {
        wm_event_t evt;
        while (wm_client_poll_event(&client, &evt)) {
            need_redraw |= ui_button_update(&back_btn, &evt);
            need_redraw |= ui_button_update(&up_btn, &evt);
            need_redraw |= ui_button_update(&down_btn, &evt);

            if (evt.type == WM_EVENT_MOUSE_CLICK && evt.button == WM_MOUSE_BUTTON_LEFT) {
                for (int i = 0; i < max_rows; i++) {
                    if (rows[i].w && rows[i].h &&
                        evt.x >= rows[i].x && evt.x < rows[i].x + rows[i].w &&
                        evt.y >= rows[i].y && evt.y < rows[i].y + rows[i].h) {

                        if (fs_mode) {
                            char out[512] = { 0 };
                            if (fs_at(out, offset + i) && out[0]) {
                                strcat(out, ":/");
                                memset(cwd, 0, sizeof(cwd));
                                strcpy(cwd, out);
                                fs_mode = false;
                                offset = 0;
                                need_redraw = 1;
                            }
                        } else {
                            dir_t dir;
                            dir_at(cwd, offset + i, &dir);
                            if (!dir.is_none) {
                                char full_path[128] = { 0 };
                                strcpy(full_path, cwd);
                                int fp_len = strlen(full_path);
                                if (fp_len > 0 && full_path[fp_len - 1] != '/') {
                                    strcat(full_path, "/");
                                }
                                strcat(full_path, dir.name);

                                if (dir.type == ENTRY_DIR) {
                                    char path_buf[256] = { 0 };
                                    if (!resolve(full_path, path_buf)) {
                                        fs_mode = true;
                                        memset(cwd, 0, sizeof(cwd));
                                        offset = 0;
                                    } else {
                                        memset(cwd, 0, sizeof(cwd));
                                        strcpy(cwd, path_buf);
                                        offset = 0;
                                    }
                                    need_redraw = 1;
                                } else if (dir.type == ENTRY_FILE) {
                                    message_send(TOPIC_DESKTOP_OPEN_FILE, full_path, strlen(full_path) + 1);
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }

        if (ui_button_clicked(&back_btn)) {
            if (!fs_mode) {
                char full_path[128] = { 0 };
                strcpy(full_path, cwd);
                int fp_len = strlen(full_path);
                if (fp_len > 0 && full_path[fp_len - 1] != '/') {
                    strcat(full_path, "/");
                }

                strcat(full_path, "..");

                char path_buf[256] = { 0 };
                if (!resolve(full_path, path_buf)) {
                    fs_mode = true;
                    memset(cwd, 0, sizeof(cwd));
                } else {
                    memset(cwd, 0, sizeof(cwd));
                    strcpy(cwd, path_buf);
                }
                offset = 0;
            }
            need_redraw = 1;
        }
        if (ui_button_clicked(&up_btn)) {
            if (offset > 0) {
                offset--;
            }
            need_redraw = 1;
        }
        if (ui_button_clicked(&down_btn)) {
            offset++;
            need_redraw = 1;
        }

        if (need_redraw) {
            int w = wm_client_width(&client);
            int h = wm_client_height(&client);
            wm_client_fill_rect(&client, 0, 0, w, h, BG_COLOR);

            wm_client_draw_string(&client, 2, 2, cwd[0] ? cwd : "(select filesystem)", 0xffffff, BG_COLOR);
            wm_client_draw_line(&client, 0, 20, w, 20, 0x444444);

            ui_button_draw(&back_btn, &client);
            ui_button_draw(&up_btn, &client);
            ui_button_draw(&down_btn, &client);

            int start_y = 32;
            memset(rows, 0, sizeof(row_area_t) * max_rows);

            if (fs_mode) {
                for (int i = 0; i < max_rows; i++) {
                    char out[512] = { 0 };
                    if (!fs_at(out, offset + i)) {
                        break;
                    }

                    int y = start_y + i * 16;
                    wm_client_draw_string(&client, 2, y, out, 0x90EE90, BG_COLOR);
                    rows[i].x = 0;
                    rows[i].y = y;
                    rows[i].w = w - 32;
                    rows[i].h = 16;
                }
            } else {
                dir_t dir;
                int count = 0;
                for (int i = offset; i < offset + max_rows && count < max_rows; i++) {
                    dir_at(cwd, i, &dir);
                    if (dir.is_none) {
                        break;
                    }

                    int y = start_y + count * 16;
                    uint32_t color = (dir.type == ENTRY_DIR) ? 0x90EE90 : 0xFF6B6B;
                    wm_client_draw_string(&client, 2, y, dir.name, color, BG_COLOR);

                    char type_ch = dir.type == ENTRY_FILE ? 'F' : 'D';
                    wm_client_draw_char(&client, w - 32, y, type_ch, 0xffffff, BG_COLOR);

                    if (dir.type == ENTRY_FILE) {
                        char full_path[128] = { 0 };
                        strcpy(full_path, cwd);
                        int fp_len = strlen(full_path);
                        if (fp_len > 0 && full_path[fp_len - 1] != '/') {
                            strcat(full_path, "/");
                        }

                        strcat(full_path, dir.name);
                        
                        int fd = open(full_path, 0);
                        if (fd >= 0) {
                            char size_str[16] = { 0 };
                            format_size(size_str, filesize(fd));
                            close(fd);
                            int size_x = w - 40 - strlen(size_str) * 8;
                            wm_client_draw_string(&client, size_x, y, size_str, 0x888888, BG_COLOR);
                        }
                    }

                    rows[count].x = 0;
                    rows[count].y = y;
                    rows[count].w = w - 32;
                    rows[count].h = 16;
                    count++;
                }
            }

            wm_client_flush(&client);
            need_redraw = 0;
        }

        yield();
    }

    free(rows);
    return 0;
}
