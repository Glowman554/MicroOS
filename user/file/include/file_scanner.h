#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
	bool (*is_file)(void* buffer, size_t size);
	void (*get_information)(void* buffer, size_t size, char* output, size_t out_size);
	char name[64];
} file_scanner_t;

extern file_scanner_t bmp_scanner;
extern file_scanner_t fpic_scanner;
extern file_scanner_t saf_scanner;
extern file_scanner_t elf_scanner;
extern file_scanner_t psf1_scanner;
extern file_scanner_t fm_scanner;
extern file_scanner_t aml_scanner;
extern file_scanner_t sm32_scanner;
extern file_scanner_t figl_scanner;
extern file_scanner_t gz_scanner;
extern file_scanner_t wad_scanner;
extern file_scanner_t mkm_scanner;
extern file_scanner_t mex_scanner;


file_scanner_t* find_file_scanner(void* buffer, size_t size);