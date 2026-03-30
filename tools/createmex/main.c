#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <zlib.h>

typedef struct mex_header_v2 {
    char header[4]; // "M2X\0"
    char programAuthor[64];
    unsigned int flags; // bit 0: compressed or not
    unsigned int abiVersion;
    unsigned int elfSizeCompressed;
} __attribute__((packed)) mex_header_v2_t;

void usage(const char* progName) {
    printf("Usage: %s [-a <author>] [-b <abi>] <elfFile> <outputFile>\n", progName);
    exit(EXIT_FAILURE);
}

void validateStringLength(const char* str, size_t maxLength) {
    if (strlen(str) > maxLength) {
        fprintf(stderr, "Error: String exceeds maximum length of %zu characters: %s\n", maxLength, str);
        exit(EXIT_FAILURE);
    }
}

unsigned char* gzip_compress(const unsigned char* data, size_t data_len, size_t* out_len) {
    uLongf bound = compressBound(data_len);
    unsigned char* compressed = malloc(bound);

    z_stream strm = {0};
    strm.next_in = (Bytef*)data;
    strm.avail_in = data_len;
    strm.next_out = compressed;
    strm.avail_out = bound;

    // 31 = 16 + MAX_WBITS => gzip with header
    if (deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        free(compressed);
        return NULL;
    }

    if (deflate(&strm, Z_FINISH) != Z_STREAM_END) {
        deflateEnd(&strm);
        free(compressed);
        return NULL;
    }

    *out_len = strm.total_out;
    deflateEnd(&strm);
    return compressed;
}

int main(int argc, char* argv[]) {
    const char* programAuthor = "ToxicFox";
    int abiVersion = 1;
    const char* elfFilePath = NULL;
    const char* outputFile = NULL;

    int idx = 1;
    while (idx < argc) {
        if (strcmp(argv[idx], "-a") == 0) {
            if (idx + 1 < argc) {
                programAuthor = argv[idx + 1];
                idx++;
            } else {
                fprintf(stderr, "Error: -a requires an argument\n");
                abort();
            }
        } else if (strcmp(argv[idx], "-b") == 0) {
            if (idx + 1 < argc) {
                abiVersion = atoi(argv[idx + 1]);
                idx++;
            } else {
                fprintf(stderr, "Error: -b requires an argument\n");
                abort();
            }
        } else if (strcmp(argv[idx], "-h") == 0) {
            usage(argv[0]);
        } else {
            if (elfFilePath == NULL) {
                elfFilePath = argv[idx];
            } else if (outputFile == NULL) {
                outputFile = argv[idx];
            } else {
                fprintf(stderr, "Error: Unrecognized argument: %s\n", argv[idx]);
                usage(argv[0]);
            }
        }
        idx++;
    }

    if (!elfFilePath) {
        fprintf(stderr, "Error: ELF input file must be specified\n");
        usage(argv[0]);
    }

    if (!outputFile) {
        fprintf(stderr, "Error: Output file must be specified\n");
        usage(argv[0]);
    }

    validateStringLength(programAuthor, 63);

    FILE* elfFile = fopen(elfFilePath, "rb");
    if (!elfFile) {
        perror("Failed to open ELF file");
        return EXIT_FAILURE;
    }
    fseek(elfFile, 0, SEEK_END);
    size_t elfSize = ftell(elfFile);
    fseek(elfFile, 0, SEEK_SET);

    unsigned char* elfData = malloc(elfSize);
    fread(elfData, 1, elfSize, elfFile);
    fclose(elfFile);

    // Compress ELF
    size_t compressedSize;
    unsigned char* compressedElf = gzip_compress(elfData, elfSize, &compressedSize);
    free(elfData);

    // Create header
    mex_header_v2_t header = {0};
    memcpy(header.header, "M2X\0", 4);
    strncpy(header.programAuthor, programAuthor, 63);
    header.flags = 1; // compressed
    header.abiVersion = abiVersion;
    header.elfSizeCompressed = (uint32_t)compressedSize;

    // Write MEX file
    FILE* outFile = fopen(outputFile, "wb");
    if (!outFile) {
        perror("Failed to open output file");
        free(compressedElf);
        return EXIT_FAILURE;
    }

    fwrite(&header, sizeof(header), 1, outFile);
    fwrite(compressedElf, 1, compressedSize, outFile);
    fclose(outFile);
    free(compressedElf);

    // printf("flags: %u\n", header.flags);
    // printf("abiVersion: %u\n", header.abiVersion);
    // printf("elfSizeCompressed: %u\n", header.elfSizeCompressed);
    // printf("MEX V2 file created successfully: %s\n", outputFile);
    return 0;
}