#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct format_scanner {
    bool (*test_format)(void* file_buffer, int file_size);
    uint32_t (*get_pixel)(void* file_buffer, int file_size, int x, int y);
    void (*get_size)(void* file_buffer, int file_size, int* width, int* height);
} format_scanner_t;

extern format_scanner_t* scanners[];

extern format_scanner_t fpic_scanner;
extern format_scanner_t bmp_scanner;
extern format_scanner_t mbif_scanner;

format_scanner_t* get_scanner(void* file_buffer, int file_size);