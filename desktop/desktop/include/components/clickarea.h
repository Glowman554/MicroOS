#pragma once

#include <stdbool.h>
#include <sys/mouse.h>


typedef struct click_area {
    int x;
    int y;
    int width;
    int height;
} click_area_t;

bool check_click_area(click_area_t* area, mouse_info_t* info);

bool do_drag_click_area(mouse_info_t* start_info, mouse_info_t* info, int* offset_x, int* offset_y);