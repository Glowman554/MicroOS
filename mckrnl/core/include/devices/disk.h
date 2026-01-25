#pragma once

#include <fs/devfs.h>
#include <stdint.h>

typedef struct raw_disk_command {
    uint8_t command;
    int disk;
    uint64_t sector;
    uint32_t sector_count;
    void* buffer;
} raw_disk_command_t;

extern devfs_file_t disk_file;