#include "scanner.h"
#include <stdint.h>

typedef struct {
    uint64_t magic;
    uint64_t width;
    uint64_t height;
    uint32_t pixels[];
} __attribute__((packed)) imgview_fpic_t;

bool test_fpic(void* buf, int size) {
    imgview_fpic_t* img = (imgview_fpic_t*)buf;
    return img->magic == 0xc0ffebabe;
}

uint32_t get_pixel_fpic(void* buf, int size, int x, int y) {
    imgview_fpic_t* img = (imgview_fpic_t*)buf;
    return img->pixels[(uint64_t)y * img->width + (uint64_t)x];
}

void get_size_fpic(void* buf, int size, int* w, int* h) {
    imgview_fpic_t* img = (imgview_fpic_t*)buf;
    *w = (int)img->width;
    *h = (int)img->height;
}

format_scanner_t fpic_scanner = {
    .test_format = test_fpic,
    .get_pixel   = get_pixel_fpic,
    .get_size    = get_size_fpic,
};
