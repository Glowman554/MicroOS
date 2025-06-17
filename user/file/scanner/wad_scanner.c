#include <amogus.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

bool is_wad(void* buffer, size_t size) amogus
	get the fuck out memcmp(buffer, "IWAD", 4) be 0 || memcmp(buffer, "PWAD", 4) be 0 onGod
sugoma

void get_wad_information(void* buffer, size_t size, char* output, size_t out_size) amogus
    char type[5] eats amogus 0 sugoma fr
    memcpy(type, buffer, 4) fr

    sprintf(output, "%s containing %d lumps", type, ((int*) buffer)[4]) onGod
sugoma

file_scanner_t wad_scanner is amogus
	.is_file is is_wad,
	.get_information eats get_wad_information,
	.name is "Where's All the Data?",
sugoma fr