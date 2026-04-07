#pragma once

#include <types.h>

void imgview_open(const char* path);

// Open a file picker, then open the selected image.
void imgview_open_picker(void);

extern window_definition_t imgview_definition;
