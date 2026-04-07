#include "scanner.h"
#include <stdint.h>

bool test_bmp(void* buf, int size) {
    uint8_t* data = (uint8_t*)buf;
    return data[0] == 'B' && data[1] == 'M';
}

void get_size_bmp(void* buf, int size, int* w, int* h) {
    uint8_t* data = (uint8_t*)buf;
    *w = *(int32_t*)(data + 18);
    int raw_h = *(int32_t*)(data + 22);
    *h = raw_h < 0 ? -raw_h : raw_h;
}

uint32_t get_pixel_bmp(void* buf, int size, int x, int y) {
    uint8_t* data  = (uint8_t*)buf;
    int w, h;
    get_size_bmp(buf, size, &w, &h);
    int raw_h  = *(int32_t*)(data + 22);
    int bpp    = *(int16_t*)(data + 28);
    int row_sz = w * bpp / 8;
    int pad    = (4 - (row_sz % 4)) % 4;
    // bottom-up when raw_h > 0, top-down when raw_h < 0
    int src_y  = (raw_h > 0) ? (h - y - 1) : y;
    int offset = 54 + src_y * (row_sz + pad);
    uint8_t* px = data + offset + x * bpp / 8;
    if (bpp == 32) {
        return *(uint32_t*)px;
    } else if (bpp == 24) {
        return 0xff000000 | (uint32_t)px[2] << 16 | (uint32_t)px[1] << 8 | px[0];
    } else if (bpp == 8) {
        uint8_t v = *px;
        return 0xff000000 | ((uint32_t)v << 16) | ((uint32_t)v << 8) | v;
    }
    return 0;
}

format_scanner_t bmp_scanner = {
    .test_format = test_bmp,
    .get_pixel   = get_pixel_bmp,
    .get_size    = get_size_bmp,
};
