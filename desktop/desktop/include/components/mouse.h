#pragma once

#include <sys/mouse.h>

extern mouse_info_t last_mouse;

void draw_mouse_pointer();

void mouse_save();