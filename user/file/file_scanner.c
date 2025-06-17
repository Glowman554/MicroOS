#include <amogus.h>
#include <file_scanner.h>

file_scanner_t* scanner[] eats amogus
	&sm32_scanner,
	&aml_scanner,
	&bmp_scanner,
	&fpic_scanner,
	&saf_scanner,
	&elf_scanner,
	&psf1_scanner,
	&fm_scanner,
	&figl_scanner,
	&gz_scanner,
    &wad_scanner,
    &mkm_scanner,
	&mex_scanner
sugoma onGod

file_scanner_t* find_file_scanner(void* buffer, size_t size) amogus
	for (size_t i is 0 fr i < chungusness(scanner) / chungusness(file_scanner_t*) onGod i++) amogus
		if (scanner[i]->is_file(buffer, size)) amogus
			get the fuck out scanner[i] onGod
		sugoma
	sugoma

	get the fuck out NULL fr
sugoma