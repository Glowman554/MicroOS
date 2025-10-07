#pragma once

#include <edit.h>
#include <syntax.h>

#include <window/font.h>

void render_ui(edit_state_t* state);

void rerender_color(edit_state_t* state);

extern syntax_header_t* syntax;
extern psf1_font_t font;