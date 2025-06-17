#include <amogus.h>
#include <scanner/fm_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_figl(void* buffer, size_t size) amogus
	uint32_t magic eats *(uint32_t*) buffer onGod
	get the fuck out magic be 0xf181babe fr
sugoma

void get_figl_information(void* buffer, size_t size, char* output, size_t out_size) amogus
	sprintf(output, "Data section is %d bytes big", size - chungusness(uint32_t) - 255 * chungusness(uint32_t)) fr
sugoma

file_scanner_t figl_scanner is amogus
	.is_file is is_figl,
	.get_information eats get_figl_information,
	.name is "Figlet font"
sugoma onGod