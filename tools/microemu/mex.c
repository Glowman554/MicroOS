#include <mex.h>
#include <tinf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t *mex_decompress(uint8_t *compressed, unsigned int comp_size, size_t *out_size) {
    unsigned int decompressed_size = compressed[comp_size - 4] | (compressed[comp_size - 3] << 8) | (compressed[comp_size - 2] << 16) | (compressed[comp_size - 1] << 24);

    uint8_t *out = malloc(decompressed_size);
    if (!out) {
        return NULL;
    }

    unsigned int destLen = decompressed_size;
    int res = tinf_gzip_uncompress(out, &destLen, compressed, comp_size);
    if (res != TINF_OK) {
        fprintf(stderr, "[mex] decompression failed: %d\n", res);
        free(out);
        return NULL;
    }

    *out_size = destLen;
    return out;
}

uint8_t *mex_load(uint8_t *file, size_t file_size, size_t *elf_size) {
    if (file_size < 4) {
        return NULL;
    }

    if (file[0] == 'M' && file[1] == '2' && file[2] == 'X' && file[3] == '\0') {
        if (file_size < sizeof(mex_header_v2_t)) {
            fprintf(stderr, "[mex] file too small for v2 header\n");
            return NULL;
        }
        mex_header_v2_t *hdr = (mex_header_v2_t *)file;
        uint8_t *payload = file + sizeof(mex_header_v2_t);
        unsigned int comp_size = hdr->elfSizeCompressed;

        // printf("[mex] v2 by '%.64s', abi=%d, flags=0x%x, compressed=%u\n", hdr->programAuthor, hdr->abiVersion, hdr->flags, comp_size);

        if (hdr->flags & M_COMPRESSED_FLAG) {
            return mex_decompress(payload, comp_size, elf_size);
        } else {
            uint8_t *out = malloc(comp_size);
            if (!out) {
                return NULL;
            }
            memcpy(out, payload, comp_size);
            *elf_size = comp_size;
            return out;
        }
    }

    if (file[0] == 'M' && file[1] == 'E' && file[2] == 'X' && file[3] == '\0') {
        if (file_size < sizeof(mex_header_t)) {
            fprintf(stderr, "[mex] file too small for v1 header\n");
            return NULL;
        }
        mex_header_t *hdr = (mex_header_t *)file;
        uint8_t *payload = file + sizeof(mex_header_t);
        unsigned int comp_size = hdr->elfSizeCompressed;

        // printf("[mex] v1 by '%.64s', compressed=%u\n", hdr->programAuthor, comp_size);

        return mex_decompress(payload, comp_size, elf_size);
    }

    uint8_t *out = malloc(file_size);
    if (!out) {
        return NULL;
    }

    memcpy(out, file, file_size);
    *elf_size = file_size;

    return out;
}
