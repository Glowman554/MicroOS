#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <buildin/disk_raw.h>
#include <crc32.h>
#include <gpt.h>

#define DIV_ROUNDUP(a, b) (((a) + ((b) - 1)) / (b))

int main(int argc, char* argv[], char* envp[]) {
    char path[128] = { 0 };
    sprintf(path, "%slimine/limine_hdd.bin", getenv("ROOT_FS"));

    printf("Loading %s...\n", path);
    FILE* img = fopen(path, "rb");
    if (img == NULL) {
        printf("Could not open %s!\n", path);
        return -1;
    }

    fsize(img, bootloader_file_size);

    uint8_t* bootloader_img = malloc(bootloader_file_size);
    fread(bootloader_img, bootloader_file_size, 1, img);
    fclose(img);

	if (argc != 2) {
		printf("Usage: %s <device_id>\n", argv[0]);
		return 1;
	}

	int device_id = atoi(argv[1]);

	char* buffer = malloc(512);
	struct gpt_table_header* header = (struct gpt_table_header*) buffer;

    read_sector_raw(device_id, 1, 1, buffer);

	if (memcmp(header->signature, "EFI PART", 8) != 0) {
		printf("Invalid signature\n");
		return 1;
	}

	printf("Secondary header at LBA 0x%d.\n", header->alternate_lba);

	char* buffer2 = malloc(512);
	struct gpt_table_header* header2 = (struct gpt_table_header*) buffer2;

    read_sector_raw(device_id, header->alternate_lba, 1, buffer2);

	if (memcmp(header2->signature, "EFI PART", 8) != 0) {
		printf("Invalid signature\n");
		return 1;
	}

	size_t stage2_size = bootloader_file_size - 512;
	size_t stage2_sects = DIV_ROUNDUP(stage2_size, 512);
	uint16_t stage2_size_a = (stage2_sects / 2) * 512 + (stage2_sects % 2 ? 512 : 0);
	uint16_t stage2_size_b = (stage2_sects / 2) * 512;
	uint64_t stage2_loc_a = 512;
	uint64_t stage2_loc_b = stage2_loc_a + stage2_size_a;

	printf("Attempting GPT embedding...\n");


	char* buffer3 = malloc(header->number_of_partition_entries * header->size_of_partition_entry + 512);
	int num_entries_sectors = header->number_of_partition_entries * header->size_of_partition_entry / 512 + 1;
    read_sector_raw(device_id, header->partition_entry_lba, num_entries_sectors, buffer3);

	int64_t max_partition_entry_used = -1;
	for (int64_t i = 0; i < (int64_t)header->number_of_partition_entries; i++) {
		struct gpt_entry* entry = (struct gpt_entry*) &buffer3[i * header->size_of_partition_entry];

		if (entry->unique_partition_guid[0] != 0 || entry->unique_partition_guid[1] != 0) {
			if (i > max_partition_entry_used) {
				max_partition_entry_used = i;
			}
		}
	}

	stage2_loc_a = (header->partition_entry_lba + 32) * 512;
	stage2_loc_a -= stage2_size_a;
	stage2_loc_a &= ~(512 - 1);
	stage2_loc_b = (header2->partition_entry_lba + 32) * 512;
	stage2_loc_b -= stage2_size_b;
	stage2_loc_b &= ~(512 - 1);

	size_t partition_entries_per_lb = 512 / header->size_of_partition_entry;
	size_t new_partition_array_lba_size = stage2_loc_a / 512 - header->partition_entry_lba;
	size_t new_partition_entry_count = new_partition_array_lba_size * partition_entries_per_lb;

	if ((int64_t)new_partition_entry_count <= max_partition_entry_used) {
		printf("ERROR: Cannot embed because there are too many used partition entries.\n");
		abort();
	}

	printf("New maximum count of partition entries: %d.\n", new_partition_entry_count);

	for (size_t i = max_partition_entry_used + 1; i < new_partition_entry_count; i++) {
		struct gpt_entry* entry = (struct gpt_entry*) &buffer3[i * header->size_of_partition_entry];
		memset(entry, 0, header->size_of_partition_entry);
	}

    write_sector_raw(device_id, header->partition_entry_lba, num_entries_sectors, buffer3);
    write_sector_raw(device_id, header2->partition_entry_lba, num_entries_sectors, buffer3);

	uint32_t crc_partition_array = crc32((uint8_t*) &buffer3[0], new_partition_entry_count * header->size_of_partition_entry);
	// printf("CRC32 of partition array: 0x%x.\n", crc_partition_array);

	header->partition_entry_array_crc32 = crc_partition_array;
	header->number_of_partition_entries = new_partition_entry_count;
	header->crc32 = 0;
	header->crc32 = crc32((uint8_t*) header, 92);

	header2->partition_entry_array_crc32 = crc_partition_array;
	header2->number_of_partition_entries = new_partition_entry_count;
	header2->crc32 = 0;
	header2->crc32 = crc32((uint8_t*) header2, 92);

    write_sector_raw(device_id, 1, 1, buffer);
    write_sector_raw(device_id, header->alternate_lba, 1, buffer2);

	printf("Stage 2 to be located at 0x%x and 0x%x.\n", stage2_loc_a, stage2_loc_b);

	char* orig_mbr = malloc(512);
	char* mod_mbr = malloc(512);

    read_sector_raw(device_id, 0, 1, orig_mbr);
    read_sector_raw(device_id, 0, 1, mod_mbr);

	memcpy(mod_mbr, &bootloader_img[0], 512);
	uint16_t* loc = (uint16_t*) &mod_mbr[0x1a4];
	*loc = stage2_size_a;
	loc = (uint16_t*) &mod_mbr[0x1a4 + 2];
	*loc = stage2_size_b;

	uint64_t* loc2 = (uint64_t*) &mod_mbr[0x1a4 + 4];
	*loc2 = stage2_loc_a;
	loc2 = (uint64_t*) &mod_mbr[0x1a4 + 12];
	*loc2 = stage2_loc_b;
	
	memcpy(&mod_mbr[218], &orig_mbr[218], 6);
	memcpy(&mod_mbr[440], &orig_mbr[440], 70);

    write_sector_raw(device_id, 0, 1, mod_mbr);

	assert(stage2_loc_a % 512 == 0);
	assert(stage2_loc_b % 512 == 0);
	assert(stage2_size_a % 512 == 0);
	assert(stage2_size_b % 512 == 0);

    write_sector_raw(device_id, stage2_loc_a / 512, stage2_size_a / 512, &bootloader_img[512]);
    write_sector_raw(device_id, stage2_loc_b / 512, stage2_size_b / 512, &bootloader_img[512 + stage2_size_a]);

	printf("Reminder: Remember to copy the limine.sys file in either\n"
			"          the root or /boot directories of one of the partitions\n"
			"          on the device, or boot will fail!\n");

	printf("Limine installed successfully!\n");

	return 0;
}