#pragma once

#include <components/clickarea.h>

#define task_bar_height 32
#define task_bar_color 0xaabbccdd
#define task_bar_color_focused 0xff11ffff

extern click_area_t logo_button;

void draw_task_bar();
void draw_task_bar_infos();