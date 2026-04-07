#include "scanner.h"
#include <stddef.h>

format_scanner_t* scanners[] = {
    &fpic_scanner,
    &bmp_scanner,
    &mbif_scanner,
    NULL
};

format_scanner_t* get_scanner(void* buf, int size) {
    for (int i = 0; scanners[i]; i++) {
        if (scanners[i]->test_format(buf, size)) {
            return scanners[i];
        }
    }
    return NULL;
}
