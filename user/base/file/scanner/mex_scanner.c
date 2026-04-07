#include <scanner/mex_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <string.h>

bool is_mex(void* buffer, size_t size) {
    mex_header_t* header = (mex_header_t*) buffer;
    return memcmp(header->header, "MEX\0", 4) == 0;
}

static unsigned int read_le32(const unsigned char *p) {
	return ((unsigned int) p[0]) | ((unsigned int) p[1] << 8) | ((unsigned int) p[2] << 16) | ((unsigned int) p[3] << 24);
}

void get_mex_information(void* buffer, size_t size, char* out, size_t out_size) {
    mex_header_t* header = (mex_header_t*) buffer;
    void* content = (void*) header + sizeof(mex_header_t);
    unsigned int decompressed_size = read_le32(content + header->elfSizeCompressed - 4);

    sprintf(out, "Program author: %s, ELF size: %dkb, compressed size: %dkb", header->programAuthor, decompressed_size / 1024, header->elfSizeCompressed / 1024);
}

file_scanner_t mex_scanner = {
	.is_file = is_mex,
	.get_information = get_mex_information,
	.name = "MicroOS executable",
};