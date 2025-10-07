#include <scanner.h>
#include <stddef.h>

#include <stdlib.h>

format_scanner_t* scanners[] = {
    &fpic_scanner,
    &bmp_scanner,
    &mbif_scanner,
    NULL
};

format_scanner_t* get_scanner(void* file_buffer, int file_size) {
    for (int i = 0; scanners[i]; i++) {
        if (scanners[i]->test_format(file_buffer, file_size)) {
            return scanners[i];
        }
    }

    // printf("Invalid file format for imgview!\n");
    abort();
}