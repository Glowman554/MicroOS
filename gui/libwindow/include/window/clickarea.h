#pragma once

#include <stdbool.h>
#include <sys/mouse.h>


typedef struct click_area {
    int x;
    int y;
    int width;
    int height;
} click_area_t;

bool check_click_area_window(click_area_t* area, mouse_info_t* info);