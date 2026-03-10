#pragma once

#include <types.h>

typedef void (*filepicker_callback_t)(const char* path);

void filepicker_open(filepicker_callback_t callback);
