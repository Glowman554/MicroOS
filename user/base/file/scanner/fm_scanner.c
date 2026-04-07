#include <scanner/fm_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_fm(void* buffer, size_t size) {
	return ((struct foxm_t*) buffer)->magic == FM_MAGIC;
}

void get_fm_information(void* buffer, size_t size, char* out, size_t out_size) {
	sprintf(out, "Contains %d notes", (size - sizeof(uint32_t)) / sizeof(struct note_t));
}

file_scanner_t fm_scanner = {
	.is_file = is_fm,
	.get_information = get_fm_information,
	.name = "Fox Music",
};