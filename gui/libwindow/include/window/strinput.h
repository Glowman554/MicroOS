#pragma once

#include <stdint.h>
#include <window/font.h>
#include <stdbool.h>

typedef struct strinput {
    int x;
    int y;

    uint32_t bgcolor;
    uint32_t fgcolor;

    char* buffer;
    int idx;
} strinput_t;

bool strinput(psf1_font_t* font, strinput_t* data);