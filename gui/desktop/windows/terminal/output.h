#pragma once

#include "terminal.h"

void term_append(terminal_state_t* st, const char* text, int len);
void term_puts(terminal_state_t* st, const char* str);
void term_printf(terminal_state_t* st, const char* fmt, ...);
void term_print_prompt(terminal_state_t* st);
