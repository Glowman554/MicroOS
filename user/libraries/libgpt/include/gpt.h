#pragma once

#include <stdint.h>
#include <stdbool.h>

struct gpt_table_header {
    char     signature[8];
    uint32_t revision;
    uint32_t header_size;
    uint32_t crc32;
    uint32_t _reserved0;

    uint64_t my_lba;
    uint64_t alternate_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;

    uint64_t disk_guid[2];

    uint64_t partition_entry_lba;
    uint32_t number_of_partition_entries;
    uint32_t size_of_partition_entry;
    uint32_t partition_entry_array_crc32;
};

struct gpt_entry {
    uint64_t partition_type_guid[2];
    uint64_t unique_partition_guid[2];
    uint64_t starting_lba;
    uint64_t ending_lba;
    uint64_t attributes;
    uint16_t partition_name[36];
};

bool create_gpt(int device_id);
bool add_gpt_partition(int device_id, uint64_t sectors, const uint8_t type_guid[16], const char* name);
bool dump_gpt(int device_id);
uint64_t parse_size(const char* text, bool* ok);
bool parse_type_guid(const char* text, uint8_t guid[16]);
