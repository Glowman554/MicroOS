#pragma once

typedef struct mex_header {
    char header[4]; // "MEX\0"
    char programAuthor[64];
    unsigned int elfSizeCompressed;
} __attribute__((packed)) mex_header_t;

typedef struct mex_header_v2 {
    char header[4]; // "M2X\0"
    char programAuthor[64];
    unsigned int flags; // bit 0: compressed or not
    unsigned int abiVersion;
    unsigned int elfSizeCompressed;
} __attribute__((packed)) mex_header_v2_t;

#define M_COMPRESSED_FLAG (1 << 0)

int load_executable(int term, char* path, char** argv, char** envp);
int init_executable(int term, void* image, char** argv, char** envp);