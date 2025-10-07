#pragma once

#include <window.h>
#include <fpic.h>

#include <components/clickarea.h>

typedef struct window_instance {
    window_t* window;
    int pid;
    fpic_image_t* icon;
    click_area_t icon_area;
    click_area_t exit_button;
    click_area_t drag_area;
    click_area_t resize_area;
    click_area_t minimize_button;
} window_instance_t;

#define max_instances 32

extern window_instance_t window_instances[];

void draw_window(window_instance_t* window);
