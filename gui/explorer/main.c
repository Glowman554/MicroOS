#include <stdlib.h>
#include <window.h>
#include <window/font.h>
#include <window/fpic.h>
#include <window/clickarea.h>
#include <string.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/env.h>

int MAX_ROWS = 16;
#define GET_CWD(cwd) char cwd[64] = { 0 }; set_env(SYS_GET_PWD_ID, cwd);

#define BGCOLOR 0x0
#define FGCOLOR 0xffffffff

psf1_font_t font;
extern fpic_image_t up_arrow;
click_area_t up_arrow_area;
extern fpic_image_t down_arrow;
click_area_t down_arrow_area;
extern fpic_image_t back_button;
click_area_t back_button_area;

click_area_t* files = NULL;

int offset = 0;
bool fs_mode = true;

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

void draw() {
    for (int i = 0; i < window->window_width; i++) {
        for (int j = 0; j < window->window_height; j++) {
            set_pixel_window(i, j, BGCOLOR);
        }
    }

    GET_CWD(cwd);
    draw_string_window(&font, 0, 0, cwd, FGCOLOR, BGCOLOR);
    draw_line_window(0, 16 + 8, window->window_width, 16 + 8, FGCOLOR);

    up_arrow_area.x = window->window_width - 16;
    up_arrow_area.y = 16 * 2;
    up_arrow_area.width = 16;
    up_arrow_area.height = 16;
    draw_fpic_window(&up_arrow, up_arrow_area.x, up_arrow_area.y);

    down_arrow_area.x = window->window_width - 16;
    down_arrow_area.y = window->window_height - 16;
    down_arrow_area.width = 16;
    down_arrow_area.height = 16;
    draw_fpic_window(&down_arrow, down_arrow_area.x, down_arrow_area.y);

    back_button_area.x = window->window_width - 25 * 2;
    back_button_area.y = 0 * 2;
    back_button_area.width = 25 * 2;
    back_button_area.height = 11 * 2;
    draw_fpic_scaled_window(&back_button, back_button_area.x, back_button_area.y, 2);

    for (int i = 0; i < MAX_ROWS; i++) {
        files[i] = (click_area_t) { 0, 0, 0, 0 };
    }

    if (fs_mode) {
        char out[512];
		memset(out, 0, 512);
		int idx = offset;
		while(fs_at(out, idx)) {
            draw_string_window(&font, 0, (2 + (idx - offset)) * 16, out, FGCOLOR, BGCOLOR);

            files[idx - offset].x = 0;
            files[idx - offset].y = (2 + (idx - offset)) * 16;
            files[idx - offset].width = window->window_width - 16 * 2;
            files[idx - offset].height = 16;

			memset(out, 0, 512);
            idx++;
        }
    } else {
        dir_t dir;
        dir_at(cwd, offset, &dir);
        while (!dir.is_none && dir.idx - offset < MAX_ROWS) {
            draw_string_window(&font, 0, (2 + (dir.idx - offset)) * 16, dir.name, FGCOLOR, BGCOLOR);
            draw_char_window(&font, window->window_width - 16 * 8, (2 + (dir.idx - offset)) * 16, dir.type == ENTRY_FILE ? 'F' : 'D', FGCOLOR, BGCOLOR);

            files[dir.idx - offset].x = 0;
            files[dir.idx - offset].y = (2 + (dir.idx - offset)) * 16;
            files[dir.idx - offset].width = window->window_width - 16 * 2;
            files[dir.idx - offset].height = 16;

            if (dir.type == ENTRY_FILE) {
                FILE* f = fopen(dir.name, "r");
                if (f) {
                    fsize(f, size);
                    fclose(f);

                    char buf[64] = { 0 };
                    format_size(buf, size);

                    draw_string_window(&font, window->window_width - 16 * 6, (2 + (dir.idx - offset)) * 16, buf, FGCOLOR, BGCOLOR);
                }
            }

            dir_at(cwd, dir.idx + 1, &dir);
        }
    }
}

int main() {
    window_init((MAX_ROWS + 2) * 16, 40 * 8, 50, 50, "Explorer");
    window_fullscreen();

    font = load_psf1_font("dev:font");

    while (true) {
        window_optimize();

        if (window_redrawn()) {
            MAX_ROWS = ((window->window_height / 16) - 2);
            files = realloc(files, sizeof(click_area_t) * MAX_ROWS);
            draw();
        }

        mouse_info_t info;
        window_mouse_info(&info);

        if (check_click_area_window(&up_arrow_area, &info)) {
            offset--;
            draw();
        }

        if (check_click_area_window(&down_arrow_area, &info)) {
            offset++;
            draw();
        }

        if (check_click_area_window(&back_button_area, &info)) {
            char path_buf[64] = { 0 };
            if (!resolve("..", path_buf)) {
                // printf("Failed to resolve???\n");
                fs_mode = true;
                draw();
                continue;
            }

            set_env(SYS_SET_PWD_ID, path_buf);
            offset = 0;

            draw();
        }

        for (int i = 0; i < MAX_ROWS; i++) {
            if (files[i].width && files[i].height) {
                if (check_click_area_window(&files[i], &info)) {
                    if (fs_mode) {
                        char out[512] = { 0 };
                        fs_at(out, offset + i);
                        strcat(out, ":/");

                        set_env(SYS_SET_PWD_ID, out);

                        fs_mode = false;
                        offset = 0;
                    } else {
                        GET_CWD(cwd);

                        dir_t dir;
                        dir_at(cwd, offset + i, &dir);

                        if (dir.type == ENTRY_FILE) {
                            // window_open_prog_request(dir.name);
                        } else {
                            char path_buf[64] = { 0 };
                            if (!resolve(dir.name, path_buf)) {
                                // printf("Failed to resolve???\n");
                                continue;
                            }

                            set_env(SYS_SET_PWD_ID, path_buf);

                            offset = 0;
                        }
                    }
                    draw();
                }
            }
        }
    }
}