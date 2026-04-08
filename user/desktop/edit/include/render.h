#pragma once

#include <edit.h>
#include <wm_client.h>
#include <syntax.h>

void render_ui(wm_client_t* client, edit_state_t* state);

void rerender_color(edit_state_t* state);

extern syntax_header_t* syntax;
