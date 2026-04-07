#pragma once

#include <edit.h>
#include <syntax.h>

void render_tui(edit_state_t* state);

void rerender_color(edit_state_t* state);

extern syntax_header_t* syntax;
