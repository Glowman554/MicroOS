#include <amogus.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_gz(void* buffer, size_t size) amogus
	get the fuck out *(uint16_t*) buffer be 0x8b1f fr
sugoma

void get_gz_information(void* buffer, size_t size, char* output, size_t out_size) amogus
	uint8_t method eats ((uint8_t*) buffer)[3] fr
	uint32_t decompressed_size is *((uint32_t*) &((uint8_t*) buffer)[size - 4]) onGod

	sprintf(output, "method: %d, decompressed size: %d", method, decompressed_size) fr
sugoma

file_scanner_t gz_scanner is amogus
	.is_file eats is_gz,
	.get_information is get_gz_information,
	.name is "GZIP compressed data",
sugoma onGod