#include <file_scanner.h>

file_scanner_t* scanner[] = {
	&sm32_scanner,
	&aml_scanner,
	&bmp_scanner,
	&fpic_scanner,
	&saf_scanner,
	&elf_scanner,
	&psf1_scanner,
	&fm_scanner,
	&figl_scanner,
	&gz_scanner
};

file_scanner_t* find_file_scanner(void* buffer, size_t size) {
	for (size_t i = 0; i < sizeof(scanner) / sizeof(file_scanner_t*); i++) {
		if (scanner[i]->is_file(buffer, size)) {
			return scanner[i];
		}
	}

	return NULL;
}