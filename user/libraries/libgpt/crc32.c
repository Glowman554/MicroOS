#include <crc32.h>
#include <stddef.h>
#include <stdint.h>

uint32_t crc32(void *_stream, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    uint8_t *data = (uint8_t*) _stream;

    while (len--) {
        crc ^= *data++;
        for (int i = 0; i < 8; i++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }

    return ~crc;
}
