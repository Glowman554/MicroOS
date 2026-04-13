#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct mex_header {
    char header[4]; // "MEX\0"
    char programAuthor[64];
    unsigned int elfSizeCompressed;
} __attribute__((packed)) mex_header_t;

typedef struct mex_header_v2 {
    char header[4]; // "M2X\0"
    char programAuthor[64];
    unsigned int flags;
    unsigned int abiVersion;
    unsigned int elfSizeCompressed;
} __attribute__((packed)) mex_header_v2_t;

#define M_COMPRESSED_FLAG (1 << 0)

uint8_t *mex_load(uint8_t *file, size_t file_size, size_t *elf_size);
