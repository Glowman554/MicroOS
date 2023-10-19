#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct raw_disk_command {
    uint8_t command;
    int disk;
    uint64_t sector;
    uint32_t sector_count;
    void* buffer;
} raw_disk_command_t;

void read_sector_raw(int disk, uint64_t sector, uint32_t sector_count, void* buffer);
void write_sector_raw(int disk, uint64_t sector, uint32_t sector_count, void* buffer);
int disk_count(bool* physical);