#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct mex_header_v2 {
    char header[4]; // "M2X\0"
    char programAuthor[64];
    unsigned int flags; // bit 0: compressed or not
    unsigned int abiVersion;
    unsigned int elfSizeCompressed;
} __attribute__((packed)) mex_header_v2_t;

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: %s <input> <program_author> <abi_version> <output>\n", argv[0]);
        return 1;
    }

    const char* input_elf = argv[1];
    const char* program_author = argv[2];
    const char* abi_version = argv[3];
    const char* output_mex = argv[4];

    FILE* elf_file = fopen(input_elf, "rb");
    if (!elf_file) {
        printf("Failed to open input ELF file: %s\n", input_elf);
        return 1;
    }

    mex_header_v2_t header;
    memcpy(header.header, "M2X", 4);
    memset(header.programAuthor, 0, sizeof(header.programAuthor));
    strcpy(header.programAuthor, program_author);
    header.flags = 0; // not compressed
    header.abiVersion = atoi(abi_version);
    header.elfSizeCompressed = 0; // not compressed
    
    fseek(elf_file, 0, SEEK_END);
    long elf_size = ftell(elf_file);
    fseek(elf_file, 0, SEEK_SET);

    void* elf_data = malloc(elf_size);
    fread(elf_data, 1, elf_size, elf_file);
    fclose(elf_file);

    FILE* mex_file = fopen(output_mex, "wb");
    if (!mex_file) {
        printf("Failed to open output MEX file: %s\n", output_mex);
        free(elf_data);
        return 1;
    }

    fwrite(&header, sizeof(header), 1, mex_file);
    fwrite(elf_data, 1, elf_size, mex_file);

    fclose(mex_file);
    free(elf_data);

    return 0;
}