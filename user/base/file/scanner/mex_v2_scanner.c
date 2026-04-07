#include <scanner/mex_v2_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <string.h>

bool is_mex_v2(void* buffer, size_t size) {
    mex_header_v2_t* header = (mex_header_v2_t*) buffer;
    return memcmp(header->header, "M2X\0", 4) == 0;
}

static unsigned int read_le32(const unsigned char *p) {
	return ((unsigned int) p[0]) | ((unsigned int) p[1] << 8) | ((unsigned int) p[2] << 16) | ((unsigned int) p[3] << 24);
}

void get_mex_v2_information(void* buffer, size_t size, char* out, size_t out_size) {
    mex_header_v2_t* header = (mex_header_v2_t*) buffer;
    void* content = (void*) header + sizeof(mex_header_v2_t);
    unsigned int decompressed_size = read_le32(content + header->elfSizeCompressed - 4);

    sprintf(out, "Program author: %s, ABI version: %d, Flags: 0x%x, ELF size: %dkb, compressed size: %dkb", header->programAuthor, header->abiVersion, header->flags, decompressed_size / 1024, header->elfSizeCompressed / 1024);
}

file_scanner_t mex_v2_scanner = {
	.is_file = is_mex_v2,
	.get_information = get_mex_v2_information,
	.name = "MicroOS executable v2",
};