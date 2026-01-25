#pragma once

typedef struct mex_header_v2 {
    char header[4]; // "M2X\0"
    char programAuthor[64];
    unsigned int flags; // bit 0: compressed or not
    unsigned int abiVersion;
    unsigned int elfSizeCompressed;
} __attribute__((packed)) mex_header_v2_t;