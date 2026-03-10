#pragma once

#include <types.h>

void desktop_draw_background(void);
void desktop_draw_all(void);

void desktop_register_file_assoc(const char* ext, void (*open_fn)(const char* path));
void desktop_open_file(const char* path);

void desktop_start_menu_init(void);
bool desktop_is_start_menu_open(void);
void desktop_toggle_start_menu(void);
bool desktop_start_menu_handle_click(int x, int y);
bool desktop_taskbar_handle_click(int x, int y);
