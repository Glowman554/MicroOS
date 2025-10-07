#include <fpic.h>
#include <render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

fpic_image_t* load_fpic(const char* file) {
    // printf("Loading %s...\n", file);
    FILE* f = fopen(file, "rb");
    if (!f) {
        // printf("Failed to load fpic %s!\n", file);
        return NULL;
    }

    fsize(f, size);

    void* buffer = malloc(size);
    fread(buffer, size, 1, f);
    fclose(f);

    fpic_image_t* fpic = (fpic_image_t*) buffer;
    if (fpic->magic == 0xc0ffebabe) {
        return fpic;
    } else {
        free(buffer);
        // printf("Failed to load fpic %s!\n", file);
        return NULL;
    }
}



uint32_t get_pixel_fpic(fpic_image_t* pic, int x, int y) {
    if (x < 0 || y < 0 || x >= pic->width || y >= pic->height) {
        return 0;
    }
    return pic->pixels[y * pic->width + x];
}

void draw_fpic(fpic_image_t* pic, int x, int y) {
    for (int i = 0; i < pic->width; i++) {
        for (int j = 0; j < pic->height; j++) {
            set_pixel(x + i, y + j, get_pixel_fpic(pic, i, j));
        }
    }
}

void draw_fpic_scaled(fpic_image_t* pic, int x, int y, int scale) {
    for (int i = 0; i < pic->width * scale; i++) {
        for (int j = 0; j < pic->height * scale; j++) {
            set_pixel(x + i, y + j, get_pixel_fpic(pic, i / scale, j / scale));
        }
    }
}