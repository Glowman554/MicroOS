#include <window.h>
#include <string.h>
#include <sys/getc.h>

#include <sys/spawn.h>
#include <buildin/framebuffer.h>
#include <buildin/mouse.h>
#include <stdlib.h>

#include <window/mouse.h>

window_t* window;

void window_init(int height, int width, int x, int y, const char* name) {
    window = (window_t*) malloc(sizeof(window_t));
    memset(window, 0, sizeof(window_t));

    window->window_height = height;
    window->window_width = width;
    window->window_x = x;
    window->window_y = y;
    window->redrawn = true;
    window->scale = 1;
    strcpy(window->name, name);

    window->info = fb_load_info();
}

void window_fullscreen() {
    window->window_height = window->info.fb_height;
    window->window_width = window->info.fb_width;
    window->window_x = 0;
    window->window_y = 0;
    window->redrawn = true;
}

char window_async_getc() {
    return async_getc();
}

char window_async_getarrw() {
    return async_getarrw();
}

void window_mouse_info(mouse_info_t* info) {
    mouse_info(info);
    draw_mouse_pointer(info);

    info->x -= window->window_x;
    info->y -= window->window_y;
}

void window_optimize() {
}

bool window_redrawn() {
    bool was_redrawn = window->redrawn;
    window->redrawn = false;
    return was_redrawn;
}

void window_clear(uint32_t color) {
    for (int i = 0; i < window->window_width; i++) {
        for (int j = 0; j < window->window_height; j++) {
        	set_pixel_window(i, j, color);
    	}
	}
}