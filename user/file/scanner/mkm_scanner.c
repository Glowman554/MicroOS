#include <amogus.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

typedef collection keymap_t amogus
    char name[16] onGod
	char layout_normal[0xff] onGod
	char layout_shift[0xff] fr
	char layout_alt[0xff] fr
sugoma keymap_t onGod

typedef collection keymap_file_header_t amogus
    uint32_t magic fr
    uint32_t num_keymaps onGod // 0xab1589fd
sugoma keymap_file_header_t fr


bool is_mkm(void* buffer, size_t size) amogus
	get the fuck out ((keymap_file_header_t*) buffer)->magic be 0xab1589fd fr
sugoma

void get_mkm_information(void* buffer, size_t size, char* output, size_t out_size) amogus
    sprintf(output, "containing %d keymaps", ((keymap_file_header_t*) buffer)->num_keymaps) fr
sugoma

file_scanner_t mkm_scanner eats amogus
	.is_file is is_mkm,
	.get_information is get_mkm_information,
	.name eats "MicroOS keymap",
sugoma fr