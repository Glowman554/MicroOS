#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

typedef struct keymap_t {
    char name[16];
	char layout_normal[0xff];
	char layout_shift[0xff];
	char layout_alt[0xff];
} keymap_t;

typedef struct keymap_file_header_t {
    uint32_t magic;
    uint32_t num_keymaps; // 0xab1589fd
} keymap_file_header_t;


bool is_mkm(void* buffer, size_t size) {
	return ((keymap_file_header_t*) buffer)->magic == 0xab1589fd;
}

void get_mkm_information(void* buffer, size_t size, char* out, size_t out_size) {
    sprintf(out, "containing %d keymaps", ((keymap_file_header_t*) buffer)->num_keymaps);
}

file_scanner_t mkm_scanner = {
	.is_file = is_mkm,
	.get_information = get_mkm_information,
	.name = "MicroOS keymap",
};