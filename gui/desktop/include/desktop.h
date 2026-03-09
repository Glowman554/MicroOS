#pragma once

#include <types.h>

void desktop_draw_background(void);
void desktop_draw_all(void);

void desktop_register_file_assoc(const char* ext, void (*open_fn)(const char* path));
void desktop_open_file(const char* path);
