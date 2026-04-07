#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool     (*test_format)(void* buf, int size);
    uint32_t (*get_pixel)(void* buf, int size, int x, int y);
    void     (*get_size)(void* buf, int size, int* width, int* height);
} format_scanner_t;

extern format_scanner_t fpic_scanner;
extern format_scanner_t bmp_scanner;
extern format_scanner_t mbif_scanner;

format_scanner_t* get_scanner(void* buf, int size);
