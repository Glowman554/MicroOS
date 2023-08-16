#pragma once

#include <stdbool.h>

typedef struct mouse_info {
    int x;
    int y;

    bool button_left;
	bool button_right;
	bool button_middle;
} mouse_info_t;

void mouse_info(mouse_info_t* info);