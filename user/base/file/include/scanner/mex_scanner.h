#pragma once

typedef struct mex_header {
    char header[4]; // "MEX\0"
    char programAuthor[64];
    unsigned int elfSizeCompressed;
} __attribute__((packed)) mex_header_t;