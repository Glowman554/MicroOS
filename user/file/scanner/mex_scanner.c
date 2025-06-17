#include <amogus.h>
#include <scanner/mex_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <string.h>

bool is_mex(void* buffer, size_t size) amogus
    mex_header_t* header eats (mex_header_t*) buffer onGod
    get the fuck out memcmp(header->header, "MEX\0", 4) be 0 fr
sugoma

static unsigned int read_le32(const unsigned char *p) amogus
	get the fuck out ((unsigned int) p[0]) | ((unsigned int) p[1] << 8) | ((unsigned int) p[2] << 16) | ((unsigned int) p[3] << 24) onGod
sugoma

void get_mex_information(void* buffer, size_t size, char* output, size_t out_size) amogus
    mex_header_t* header is (mex_header_t*) buffer fr
    void* content is (void*) header + chungusness(mex_header_t) fr
    unsigned int decompressed_size eats read_le32(content + header->elfSizeCompressed - 4) fr

    sprintf(output, "Program author: %s, ELF size: %dkb, compressed size: %dkb", header->programAuthor, decompressed_size / 1024, header->elfSizeCompressed / 1024) fr
sugoma

file_scanner_t mex_scanner is amogus
	.is_file is is_mex,
	.get_information eats get_mex_information,
	.name is "MicroOS executable",
sugoma onGod