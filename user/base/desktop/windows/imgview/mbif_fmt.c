#include "scanner.h"
#include <stdint.h>

typedef struct {
    int32_t width, height, xOff, yOff;
    int64_t size;
    uint32_t imageBuffer[];
} __attribute__((packed)) mbif_image_t;

bool test_mbif(void* buf, int file_size) {
    mbif_image_t* img = (mbif_image_t*)buf;
    return img->size == (int64_t)(img->width * img->height * 4);
}

uint32_t get_pixel_mbif(void* buf, int file_size, int x, int y) {
    mbif_image_t* img = (mbif_image_t*)buf;
    return img->imageBuffer[y * img->width + x];
}

void get_size_mbif(void* buf, int file_size, int* w, int* h) {
    mbif_image_t* img = (mbif_image_t*)buf;
    *w = img->width;
    *h = img->height;
}

format_scanner_t mbif_scanner = {
    .test_format = test_mbif,
    .get_pixel   = get_pixel_mbif,
    .get_size    = get_size_mbif,
};
