#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

bool is_wad(void* buffer, size_t size) {
	return memcmp(buffer, "IWAD", 4) == 0 || memcmp(buffer, "PWAD", 4) == 0;
}

void get_wad_information(void* buffer, size_t size, char* out, size_t out_size) {
    char type[5] = { 0 };
    memcpy(type, buffer, 4);

    sprintf(out, "%s containing %d lumps", type, ((int*) buffer)[4]);
}

file_scanner_t wad_scanner = {
	.is_file = is_wad,
	.get_information = get_wad_information,
	.name = "Where's All the Data?",
};