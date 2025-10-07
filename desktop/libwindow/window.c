#include <window.h>
#include <string.h>
#include <sys/getc.h>

#include <sys/spawn.h>
#include <stdio.h>

window_t* window = (window_t*) 0xF0002000;

void window_init(int height, int width, int x, int y, const char* name) {
    window->window_height = height;
    window->window_width = width;
    window->window_x = x;
    window->window_y = y;
    strcpy(window->name, name);

    window->ready = true;
    window->dirty = true;

    while (window->dirty) {
        yield();
    }
}

char window_async_getc() {
    if (window->focus) {
        return async_getc();
    }
    return 0;
}

char window_async_getarrw() {
    if (window->focus) {
        return async_getarrw();
    }
    return 0;
}

void window_mouse_info(mouse_info_t* info) {
    if (window->focus) {
        mouse_info(info);
        info->x -= window->window_x;
        info->y -= window->window_y;
        return;
    }
    *info = (mouse_info_t) {
        .button_left = 0,
        .button_middle = 0,
        .button_right = 0,
        .x = -1,
        .y = -1
    };
}

void window_optimize() {
    while (window->minimized) {
        yield();
    }
}

bool window_redrawn() {
    if (window->redrawn) {
        window->redrawn = false;
        return true;
    }

    return false;
}

void window_open_prog_request(const char* file) {
    memset(window->open_prog_for, 0, sizeof(window->open_prog_for));
    if (resolve((char*) file, window->open_prog_for)) {
        window->open_prog_request = true;
    }
    while (window->open_prog_request) {
        yield();
    }
}

void window_clear(uint32_t color) {
    for (int i = 0; i < window->window_width; i++) {
        for (int j = 0; j < window->window_height; j++) {
        	set_pixel_window(i, j, color);
    	}
	}
}