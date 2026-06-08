#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <non-standard/buildin/disk_raw.h>
#include <crc32.h>
#include <gpt.h>

#define SECTOR_SIZE 512
#define GPT_ENTRY_SIZE 128
#define GPT_ENTRIES 128
#define GPT_ALIGNMENT_SECTORS 2048

uint64_t align_up(uint64_t value, uint64_t alignment) {
    if (alignment == 0) {
        return value;
    }
    return ((value + alignment - 1) / alignment) * alignment;
}

bool parse_decimal_uint64(const char* text, uint64_t* value) {
    if (text == NULL || *text == '\0') {
        return false;
    }

    uint64_t result = 0;
    uint64_t max_value = (uint64_t)-1;
    const char* current = text;
    while (*current != '\0') {
        if (*current < '0' || *current > '9') {
            return false;
        }
        uint64_t digit = (uint64_t)(*current - '0');
        if (result > (max_value - digit) / 10) {
            return false;
        }
        result = result * 10 + digit;
        current++;
    }

    *value = result;
    return true;
}

uint64_t parse_size(const char* text, bool* ok) {
    if (text == NULL || *text == '\0') {
        *ok = false;
        return 0;
    }

    char* end = NULL;
    uint64_t value = 0;
    if (!parse_decimal_uint64(text, &value)) {
        *ok = false;
        return 0;
    }

    end = (char*)text;
    while (*end >= '0' && *end <= '9') {
        end++;
    }

    if (*end == '\0') {
        *ok = true;
        return value;
    }

    *ok = false;
    return 0;
}

bool hex_digit(char c, uint8_t* value) {
    if (c >= '0' && c <= '9') {
        *value = c - '0';
        return true;
    }
    if (c >= 'a' && c <= 'f') {
        *value = c - 'a' + 10;
        return true;
    }
    if (c >= 'A' && c <= 'F') {
        *value = c - 'A' + 10;
        return true;
    }
    return false;
}

bool parse_hex_byte(const char* text, uint8_t* value) {
    uint8_t high;
    uint8_t low;
    if (!hex_digit(text[0], &high) || !hex_digit(text[1], &low)) {
        return false;
    }
    *value = (uint8_t)((high << 4) | low);
    return true;
}

bool parse_guid(const char* text, uint8_t guid[16]) {
    if (text == NULL || strlen(text) != 36) {
        return false;
    }
    if (text[8] != '-' || text[13] != '-' || text[18] != '-' || text[23] != '-') {
        return false;
    }

    uint32_t part1 = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t nibble;
        if (!hex_digit(text[i], &nibble)) {
            return false;
        }
        part1 = (part1 << 4) | nibble;
    }

    guid[0] = (uint8_t)(part1 & 0xFF);
    guid[1] = (uint8_t)((part1 >> 8) & 0xFF);
    guid[2] = (uint8_t)((part1 >> 16) & 0xFF);
    guid[3] = (uint8_t)((part1 >> 24) & 0xFF);

    uint16_t part2 = 0;
    for (int i = 9; i < 13; i++) {
        uint8_t nibble;
        if (!hex_digit(text[i], &nibble)) {
            return false;
        }
        part2 = (uint16_t)((part2 << 4) | nibble);
    }
    guid[4] = (uint8_t)(part2 & 0xFF);
    guid[5] = (uint8_t)((part2 >> 8) & 0xFF);

    uint16_t part3 = 0;
    for (int i = 14; i < 18; i++) {
        uint8_t nibble;
        if (!hex_digit(text[i], &nibble)) {
            return false;
        }
        part3 = (uint16_t)((part3 << 4) | nibble);
    }
    guid[6] = (uint8_t)(part3 & 0xFF);
    guid[7] = (uint8_t)((part3 >> 8) & 0xFF);

    if (!parse_hex_byte(&text[19], &guid[8]) || !parse_hex_byte(&text[21], &guid[9])) {
        return false;
    }

    for (int i = 0; i < 6; i++) {
        if (!parse_hex_byte(&text[24 + i * 2], &guid[10 + i])) {
            return false;
        }
    }

    return true;
}

void set_guid_alias(const char* alias, uint8_t guid[16]) {
    const uint8_t basic_data[16] = {
        0xA2, 0xA0, 0xD0, 0xEB, 0xE5, 0xB9, 0x33, 0x44,
        0x87, 0xC0, 0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7
    };
    const uint8_t efi_system[16] = {
        0x28, 0x73, 0x2A, 0xC1, 0x1F, 0xF8, 0xD2, 0x11,
        0xBA, 0x4B, 0x00, 0xA0, 0xC9, 0x3E, 0xC9, 0x3B
    };

    if (strcmp(alias, "efi") == 0 || strcmp(alias, "efi-system") == 0 || strcmp(alias, "esp") == 0) {
        memcpy(guid, efi_system, 16);
    } else {
        memcpy(guid, basic_data, 16);
    }
}

bool parse_type_guid(const char* text, uint8_t guid[16]) {
    if (text == NULL || *text == '\0') {
        return false;
    }

    if (parse_guid(text, guid)) {
        return true;
    }

    set_guid_alias(text, guid);
    return true;
}

void make_random_guid(uint8_t guid[16], uint32_t seed) {
    uint32_t value = seed;
    for (int i = 0; i < 16; i++) {
        value = value * 1664525u + 1013904223u;
        guid[i] = (uint8_t)((value >> 16) & 0xFF);
    }
    guid[6] = (guid[6] & 0x0F) | 0x40;
    guid[8] = (guid[8] & 0x3F) | 0x80;
}

void write_protective_mbr(uint8_t* buffer, uint64_t disk_sectors) {
    memset(buffer, 0, SECTOR_SIZE);
    buffer[0x1BE + 4] = 0xEE;
    buffer[0x1BE + 1] = 0xFF;
    buffer[0x1BE + 2] = 0xFF;
    buffer[0x1BE + 3] = 0xFF;
    buffer[0x1BE + 5] = 0xFF;
    buffer[0x1BE + 6] = 0xFF;
    buffer[0x1BE + 7] = 0xFF;

    uint32_t start_lba = 1;
    uint32_t size_lba = (disk_sectors - 1 > 0xFFFFFFFFu) ? 0xFFFFFFFFu : (uint32_t)(disk_sectors - 1);
    memcpy(&buffer[0x1BE + 8], &start_lba, 4);
    memcpy(&buffer[0x1BE + 12], &size_lba, 4);

    buffer[510] = 0x55;
    buffer[511] = 0xAA;
}

void write_gpt_header_sector(uint8_t* buffer, struct gpt_table_header* header) {
    memset(buffer, 0, SECTOR_SIZE);
    memcpy(buffer, header, header->header_size);
}

bool read_gpt_header(int device_id, struct gpt_table_header* header) {
    uint8_t* sector = malloc(SECTOR_SIZE);
    if (sector == NULL) {
        return false;
    }

    read_sector_raw(device_id, 1, 1, sector);
    memcpy(header, sector, sizeof(*header));
    free(sector);

    if (memcmp(header->signature, "EFI PART", 8) != 0) {
        return false;
    }

    return true;
}

bool load_gpt_entries(int device_id, struct gpt_table_header* header, uint8_t* entries, uint32_t sectors) {
    if (entries == NULL || header == NULL) {
        return false;
    }

    read_sector_raw(device_id, header->partition_entry_lba, sectors, entries);
    return true;
}

bool commit_gpt_headers(int device_id, struct gpt_table_header* primary, struct gpt_table_header* secondary) {
    uint8_t* sector = malloc(SECTOR_SIZE);
    if (sector == NULL) {
        return false;
    }

    write_gpt_header_sector(sector, primary);
    write_sector_raw(device_id, primary->my_lba, 1, sector);

    write_gpt_header_sector(sector, secondary);
    write_sector_raw(device_id, secondary->my_lba, 1, sector);

    free(sector);
    return true;
}

bool dump_gpt(int device_id) {
    struct gpt_table_header primary;
    if (!read_gpt_header(device_id, &primary)) {
        printf("gpt: disk %d does not contain a valid GPT\n", device_id);
        return false;
    }

    uint32_t entry_sectors = (primary.number_of_partition_entries * primary.size_of_partition_entry + SECTOR_SIZE - 1) / SECTOR_SIZE;
    uint8_t* entries = malloc(entry_sectors * SECTOR_SIZE);
    if (entries == NULL) {
        printf("gpt: failed to allocate partition entry array\n");
        return false;
    }

    if (!load_gpt_entries(device_id, &primary, entries, entry_sectors)) {
        printf("gpt: failed to read partition entries\n");
        free(entries);
        return false;
    }

    uint64_t start = align_up(primary.first_usable_lba, GPT_ALIGNMENT_SECTORS);

    printf("GPT disk %d\n", device_id);
    printf("  usable: %d..%d\n", (unsigned long)primary.first_usable_lba, (unsigned long)primary.last_usable_lba);
    printf("  partition entries: %u\n", primary.number_of_partition_entries);
    printf("  entry size: %d bytes\n", primary.size_of_partition_entry);
    printf("  biggest partition size: %d sectors\n", (unsigned long)(primary.last_usable_lba - start + 1));
    printf("  partitions:\n");
    printf("\t#\tstart\tend\tsize\n");

    for (uint32_t i = 0; i < primary.number_of_partition_entries; i++) {
        struct gpt_entry* entry = (struct gpt_entry*)&entries[i * primary.size_of_partition_entry];
        if (entry->starting_lba == 0 && entry->ending_lba == 0) {
            continue;
        }

        uint64_t size = entry->ending_lba - entry->starting_lba + 1;
        printf("\t%d\t%d\t%d\t%d\n",
            i + 1,
            (unsigned long)entry->starting_lba,
            (unsigned long)entry->ending_lba,
            (unsigned long)size
        );
    }

    free(entries);
    return true;
}

bool create_gpt(int device_id) {
    uint64_t disk_sectors = disk_get_sector_count(device_id);
    if (disk_sectors < 34) {
        printf("gpt: disk %d is too small for GPT (%d sectors)\n", device_id, (unsigned long)disk_sectors);
        return false;
    }

    struct gpt_table_header existing;
    if (read_gpt_header(device_id, &existing)) {
        printf("gpt: GPT already exists on disk %d\n", device_id);
        return false;
    }

    uint32_t entry_sectors = (GPT_ENTRIES * GPT_ENTRY_SIZE + SECTOR_SIZE - 1) / SECTOR_SIZE;
    uint64_t first_usable = 2 + entry_sectors;
    uint64_t last_usable = disk_sectors - entry_sectors - 2;

    uint8_t* entries = malloc(entry_sectors * SECTOR_SIZE);
    if (entries == NULL) {
        printf("gpt: failed to allocate partition entry array\n");
        return false;
    }
    memset(entries, 0, entry_sectors * SECTOR_SIZE);

    uint8_t disk_guid[16];
    make_random_guid(disk_guid, (uint32_t)device_id ^ 0xA5A5A5A5);

    struct gpt_table_header primary = {0};
    memcpy(primary.signature, "EFI PART", 8);
    primary.revision = 0x00010000;
    primary.header_size = 92;
    primary.my_lba = 1;
    primary.alternate_lba = disk_sectors - 1;
    primary.first_usable_lba = first_usable;
    primary.last_usable_lba = last_usable;
    memcpy(&primary.disk_guid, disk_guid, 16);
    primary.partition_entry_lba = 2;
    primary.number_of_partition_entries = GPT_ENTRIES;
    primary.size_of_partition_entry = GPT_ENTRY_SIZE;
    primary.partition_entry_array_crc32 = crc32(entries, GPT_ENTRIES * GPT_ENTRY_SIZE);
    primary.crc32 = 0;
    primary.crc32 = crc32(&primary, primary.header_size);

    struct gpt_table_header secondary = primary;
    secondary.my_lba = disk_sectors - 1;
    secondary.alternate_lba = 1;
    secondary.partition_entry_lba = disk_sectors - 1 - entry_sectors;
    secondary.crc32 = 0;
    secondary.crc32 = crc32(&secondary, secondary.header_size);

    uint8_t* sector = malloc(SECTOR_SIZE);
    if (sector == NULL) {
        free(entries);
        return false;
    }

    write_protective_mbr(sector, disk_sectors);
    write_sector_raw(device_id, 0, 1, sector);

    write_sector_raw(device_id, primary.partition_entry_lba, entry_sectors, entries);
    write_sector_raw(device_id, secondary.partition_entry_lba, entry_sectors, entries);

    write_gpt_header_sector(sector, &primary);
    write_sector_raw(device_id, primary.my_lba, 1, sector);

    write_gpt_header_sector(sector, &secondary);
    write_sector_raw(device_id, secondary.my_lba, 1, sector);

    free(entries);
    free(sector);

    printf("gpt: created GPT on disk %d (%d sectors)\n", device_id, (long)disk_sectors);
    
    return true;
}

void sort_boundaries(uint64_t boundaries[][2], size_t count) {
    for (size_t i = 1; i < count; i++) {
        uint64_t start = boundaries[i][0];
        uint64_t end = boundaries[i][1];
        size_t j = i;
        while (j > 0 && boundaries[j - 1][0] > start) {
            boundaries[j][0] = boundaries[j - 1][0];
            boundaries[j][1] = boundaries[j - 1][1];
            j--;
        }
        boundaries[j][0] = start;
        boundaries[j][1] = end;
    }
}

bool add_gpt_partition(int device_id, uint64_t sectors, const uint8_t type_guid[16], const char* name) {
    if (sectors == 0) {
        printf("gpt: invalid size\n");
        return false;
    }

    struct gpt_table_header primary;
    if (!read_gpt_header(device_id, &primary)) {
        printf("gpt: disk %d does not contain a valid GPT\n", device_id);
        return false;
    }

    uint32_t entry_sectors = (primary.number_of_partition_entries * primary.size_of_partition_entry + SECTOR_SIZE - 1) / SECTOR_SIZE;
    uint8_t* entries = malloc(entry_sectors * SECTOR_SIZE);
    if (entries == NULL) {
        printf("gpt: failed to allocate partition entry array\n");
        return false;
    }

    load_gpt_entries(device_id, &primary, entries, entry_sectors);

    uint64_t first_usable = primary.first_usable_lba;
    uint64_t last_usable = primary.last_usable_lba;
    uint64_t boundaries[GPT_ENTRIES][2];
    size_t used_count = 0;

    for (uint32_t i = 0; i < primary.number_of_partition_entries; i++) {
        struct gpt_entry* entry = (struct gpt_entry*)&entries[i * primary.size_of_partition_entry];
        if (entry->starting_lba != 0 && entry->ending_lba != 0 && entry->starting_lba <= entry->ending_lba) {
            boundaries[used_count][0] = entry->starting_lba;
            boundaries[used_count][1] = entry->ending_lba;
            used_count++;
        }
    }

    sort_boundaries(boundaries, used_count);

    uint64_t start = align_up(first_usable, GPT_ALIGNMENT_SECTORS);
    bool found = false;

    for (size_t index = 0; index <= used_count; index++) {
        uint64_t limit = (index < used_count) ? boundaries[index][0] - 1 : last_usable;
        if (start + sectors - 1 <= limit) {
            found = true;
            break;
        }
        if (index == used_count) {
            break;
        }
        start = align_up(boundaries[index][1] + 1, GPT_ALIGNMENT_SECTORS);
        if (start > last_usable) {
            break;
        }
    }

    if (!found) {
        printf("gpt: not enough free space for %d sectors\n", (unsigned long)sectors);
        free(entries);
        return false;
    }

    uint64_t end = start + sectors - 1;
    if (end > last_usable) {
        printf("gpt: requested partition extends beyond usable range\n");
        free(entries);
        return false;
    }

    int entry_index = -1;
    for (uint32_t i = 0; i < primary.number_of_partition_entries; i++) {
        struct gpt_entry* entry = (struct gpt_entry*)&entries[i * primary.size_of_partition_entry];
        if (entry->starting_lba == 0 && entry->ending_lba == 0) {
            entry_index = i;
            break;
        }
    }

    if (entry_index < 0) {
        printf("gpt: no available GPT partition entries\n");
        free(entries);
        return false;
    }

    struct gpt_entry* new_entry = (struct gpt_entry*)&entries[entry_index * primary.size_of_partition_entry];
    memset(new_entry, 0, primary.size_of_partition_entry);
    memcpy(new_entry->partition_type_guid, type_guid, 16);
    make_random_guid((uint8_t*)new_entry->unique_partition_guid, (uint32_t)(device_id + entry_index * 31));
    new_entry->starting_lba = start;
    new_entry->ending_lba = end;
    new_entry->attributes = 0;

    if (name != NULL) {
        size_t len = strlen(name);
        size_t count = len < 36 ? len : 36;
        for (size_t i = 0; i < count; i++) {
            new_entry->partition_name[i] = (uint16_t)(uint8_t)name[i];
        }
    }

    primary.partition_entry_array_crc32 = crc32(entries, primary.number_of_partition_entries * primary.size_of_partition_entry);
    primary.crc32 = 0;
    primary.crc32 = crc32(&primary, primary.header_size);

    struct gpt_table_header secondary = primary;
    secondary.my_lba = primary.alternate_lba;
    secondary.alternate_lba = primary.my_lba;
    secondary.partition_entry_lba = secondary.my_lba - entry_sectors;
    secondary.crc32 = 0;
    secondary.crc32 = crc32(&secondary, secondary.header_size);

    write_sector_raw(device_id, primary.partition_entry_lba, entry_sectors, entries);
    write_sector_raw(device_id, secondary.partition_entry_lba, entry_sectors, entries);
    commit_gpt_headers(device_id, &primary, &secondary);

    printf("gpt: created partition %d on disk %d with size %d\n", entry_index + 1, device_id, (int)sectors);

    free(entries);
    return true;
}
