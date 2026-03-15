#pragma once

#include <types.h>
#include <non-standard/buildin/data/array.h>

extern window_definition_t** window_definitions;

void register_window(window_definition_t* def);
void register_windows(void);
