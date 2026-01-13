#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <window/font.h>

extern int vc_cursor_x;
extern int vc_cursor_y;

extern uint32_t vc_bgcolor;
extern uint32_t vc_color;

void vconsole_init(psf1_font_t* font);

void vconsole_clear();

bool vconsole_ansi_process(char c);

void vconsole_putc(char c);
void vconsole_puts(const char* s);
void vconsole_set_color(uint32_t color, bool background);