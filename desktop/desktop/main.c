#include <assert.h>
#include <stdio.h>
#include <sys/graphics.h>
#include <sys/file.h>
#include <sys/mouse.h>
#include <buildin/unix_time.h>
#include <buildin/framebuffer.h>
#include <sys/time.h>
#include <sys/spawn.h>
#include <sys/mmap.h>
#include <sys/getc.h>
#include <sys/env.h>
#include <stdlib.h>

#include <render.h>
#include <fpic.h>
#include <launcher.h>
#include <config.h>

#include <components/task_bar.h>
#include <components/mouse.h>
#include <components/window.h>
#include <components/launcher.h>

fb_info_t info;
psf1_font_t font;

fpic_image_t* background;
fpic_image_t* logo;
fpic_image_t* exit_button;
fpic_image_t* minimize_button;

config_node_t* config;

int SCALE = 1;

bool desktop_focus = false;

void set_cwd() {
    char buffer[256] = { 0 };
    const char* rootfs = getenv("ROOT_FS");
    sprintf(buffer, "%sopt/desktop", rootfs);
    set_env(SYS_SET_PWD_ID, buffer);
}

void load_font() {
    font = load_psf1_font("dev:font");
}

void init_config() {
    FILE* cfgFile = fopen("desktop.cfg", "r");
    assert(cfgFile);

    fsize(cfgFile, size);

    char* cfgStr = malloc(size + 1);
    cfgStr[size] = 0;
    fread(cfgStr, size, 1, cfgFile);
    fclose(cfgFile);

    config = load_cfg(cfgStr);
    free(cfgStr);
}

void init() {
    set_cwd();
    init_config();

    info = fb_load_info();
    SCALE = atoi(get_cfg_value(config, "desktop_scale"));

    info.fb_width /= SCALE;
    info.fb_height /= SCALE;

    // printf("PITCH: %d\n", info.fb_pitch);
    // printf("WIDTH: %d\n", info.fb_width);
    // printf("HEIGHT: %d\n", info.fb_height);
    // printf("BBP: %d\n", info.fb_bpp);
    // printf("ADDR: %x\n", info.fb_addr);

    fpic_image_t* loading = load_fpic("icons/loading.fpic");
    int lx = (info.fb_width - 48 * 4) / 2;
    int ly = (info.fb_height - 48 * 4) / 2;
    draw_fpic_scaled(loading, lx, ly, 4);
    free(loading);

    load_font();
    logo = load_fpic("icons/logo.fpic");
    background = load_fpic("icons/bgimage.fpic");
    exit_button = load_fpic("icons/exit.fpic");
    minimize_button = load_fpic("icons/minimize.fpic");

    init_program_launcher();
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void redraw_all() {
    set_env(SYS_ENV_PIN, (void*) 1);

    // memset((void*) info.fb_addr, 0x00, info.fb_pitch * info.fb_height);

    int lx = (info.fb_width - background->width) / 2;
    // draw_fpic_scaled(background, lx, 0, 1);

    for (int i = 0; i < info.fb_width; i++) {
        for (int j = 0; j < info.fb_height; j++) {
            set_pixel(i, j, get_pixel_fpic(background, i - lx, j));
        }
    }

    draw_desktop_program_launcher();

    for (int i = 0; i < max_instances; i++) {
        if (window_instances[i].window) {
            if (!window_instances[i].window->minimized) {
                draw_window(&window_instances[i]);
            }
            window_instances[i].window->redrawn = true;
        }
    }


    draw_task_bar();

    mouse_save();

    set_env(SYS_ENV_PIN, (void*) 0);
}

void unfocus_all() {
    desktop_focus = false;
    for (int i = 0; i < max_instances; i++) {
        if (window_instances[i].window) {
            if (!window_instances[i].window->minimized) {
                draw_window(&window_instances[i]);
            }
            window_instances[i].window->focus = false;
        }
    }
}

void check_icon_clicks() {
    if (check_click_area(&logo_button, &last_mouse)) {
        unfocus_all();
        desktop_focus = true;
        redraw_all();
        return;
    }

    for (int i = 0; i < max_instances; i++) {
        if (window_instances[i].window) {
            if (check_click_area(&window_instances[i].icon_area, &last_mouse)) {
                unfocus_all();
                window_instances[i].window->focus = true;
                window_instances[i].window->minimized = false;
                redraw_all();
                return;
            }
        }
    }
}

int main() {
    init();

    redraw_all();

    // launch_child("initrd:/bin/explorer.elf", "initrd:/envs/explorer.env", "initrd:/icons/logo.fpic", NULL);
    // launch_child("initrd:/bin/doom.elf", "initrd:/envs/doom.env", "initrd:/icons/doom.fpic", NULL);

    while (true) {
        cleanup_tasks();

        draw_mouse_pointer();
        draw_task_bar_infos();

        check_icon_clicks();

        if (desktop_focus) {
            launcher_tick();
        }

        for (int i = 0; i < max_instances; i++) {
            if (!window_instances[i].window) {
                continue;
            }
            if (window_instances[i].window->minimized) {
                continue;
            }
            if (window_instances[i].window->dirty) {
                redraw_all();
            }
            if (check_click_area(&window_instances[i].minimize_button, &last_mouse)) {
                window_instances[i].window->minimized = true;
                redraw_all();
                continue;
            }

            if (check_click_area(&window_instances[i].drag_area, &last_mouse)) {
                mouse_info_t start_pos = last_mouse;
                int window_start_x = window_instances[i].window->window_x;
                int window_start_y = window_instances[i].window->window_y;

                unfocus_all();

                while (last_mouse.button_left) {
                    draw_mouse_pointer();
                    draw_window(&window_instances[i]);

                    window_instances[i].window->window_x = window_start_x + (last_mouse.x - start_pos.x);
                    window_instances[i].window->window_y = window_start_y + (last_mouse.y - start_pos.y);
                }

                window_instances[i].window->dirty = true;
                window_instances[i].window->focus = true;
            }
            if (check_click_area(&window_instances[i].resize_area, &last_mouse)) {
                mouse_info_t start_pos = last_mouse;
                int window_start_width = window_instances[i].window->window_width;
                int window_start_height = window_instances[i].window->window_height;

                unfocus_all();

                while (last_mouse.button_left) {
                    draw_mouse_pointer();
                    draw_window(&window_instances[i]);

                    window_instances[i].window->window_width = window_start_width + (last_mouse.x - start_pos.x);
                    window_instances[i].window->window_height = window_start_height + (last_mouse.y - start_pos.y);
                }

                window_instances[i].window->dirty = true;
                window_instances[i].window->focus = true;
            }
            if (window_instances[i].window->open_prog_request) {
                window_instances[i].window->open_prog_request = false;
                launch_program_for_file_extension(window_instances[i].window->open_prog_for);
            }
        }
        yield();
    }
}