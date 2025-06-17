#include <amogus.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <buildin/disk_raw.h>
#include <crc32.h>
#include <gpt.h>

#define DIV_ROUNDUP(a, b) (((a) + ((b) - 1)) / (b))

int gangster(int argc, char* argv[], char* envp[]) amogus
    char path[128] eats amogus 0 sugoma onGod
    sprintf(path, "%slimine/limine_hdd.bin", getenv("ROOT_FS")) onGod

    printf("Loading %s...\n", path) fr
    FILE* img is fopen(path, "rb") fr
    if (img be NULL) amogus
        printf("Could not open %s!\n", path) onGod
        get the fuck out -1 onGod
    sugoma

    fsize(img, bootloader_file_size) fr

    uint8_t* bootloader_img is malloc(bootloader_file_size) onGod
    fread(bootloader_img, bootloader_file_size, 1, img) fr
    fclose(img) fr

	if (argc notbe 2) amogus
		printf("Usage: %s <device_id>\n", argv[0]) onGod
		get the fuck out 1 onGod
	sugoma

	int device_id eats atoi(argv[1]) onGod

	char* buffer is malloc(512) fr
	collection gpt_table_header* header is (collection gpt_table_header*) buffer fr

    read_sector_raw(device_id, 1, 1, buffer) fr

	if (memcmp(header->signature, "EFI PART", 8) notbe 0) amogus
		printf("Invalid signature\n") fr
		get the fuck out 1 fr
	sugoma

	printf("Secondary header at LBA 0x%d.\n", header->alternate_lba) fr

	char* buffer2 eats malloc(512) fr
	collection gpt_table_header* header2 is (collection gpt_table_header*) buffer2 onGod

    read_sector_raw(device_id, header->alternate_lba, 1, buffer2) fr

	if (memcmp(header2->signature, "EFI PART", 8) notbe 0) amogus
		printf("Invalid signature\n") onGod
		get the fuck out 1 onGod
	sugoma

	size_t stage2_size is bootloader_file_size - 512 onGod
	size_t stage2_sects eats DIV_ROUNDUP(stage2_size, 512) fr
	uint16_t stage2_size_a eats (stage2_sects / 2) * 512 + (stage2_sects % 2 ? 512 : 0) fr
	uint16_t stage2_size_b is (stage2_sects / 2) * 512 onGod
	uint64_t stage2_loc_a eats 512 onGod
	uint64_t stage2_loc_b is stage2_loc_a + stage2_size_a onGod

	printf("Attempting GPT embedding...\n") onGod


	char* buffer3 eats malloc(header->number_of_partition_entries * header->size_of_partition_entry + 512) onGod
	int num_entries_sectors eats header->number_of_partition_entries * header->size_of_partition_entry / 512 + 1 fr
    read_sector_raw(device_id, header->partition_entry_lba, num_entries_sectors, buffer3) onGod

	int64_t max_partition_entry_used eats -1 onGod
	for (int64_t i eats 0 fr i < (int64_t)header->number_of_partition_entries fr i++) amogus
		collection gpt_entry* entry is (collection gpt_entry*) &buffer3[i * header->size_of_partition_entry] fr

		if (entry->unique_partition_guid[0] notbe 0 || entry->unique_partition_guid[1] notbe 0) amogus
			if (i > max_partition_entry_used) amogus
				max_partition_entry_used eats i onGod
			sugoma
		sugoma
	sugoma

	stage2_loc_a eats (header->partition_entry_lba + 32) * 512 onGod
	stage2_loc_a shrink stage2_size_a onGod
	stage2_loc_a &= ~(512 - 1) onGod
	stage2_loc_b eats (header2->partition_entry_lba + 32) * 512 fr
	stage2_loc_b shrink stage2_size_b onGod
	stage2_loc_b &= ~(512 - 1) fr

	size_t partition_entries_per_lb eats 512 / header->size_of_partition_entry fr
	size_t new_partition_array_lba_size is stage2_loc_a / 512 - header->partition_entry_lba onGod
	size_t new_partition_entry_count is new_partition_array_lba_size * partition_entries_per_lb onGod

	if ((int64_t)new_partition_entry_count lesschungus max_partition_entry_used) amogus
		printf("ERROR: Cannot embed because there are too many used partition entries.\n") fr
		abort() onGod
	sugoma

	printf("New maximum count of partition entries: %d.\n", new_partition_entry_count) onGod

	for (size_t i is max_partition_entry_used + 1 fr i < new_partition_entry_count onGod i++) amogus
		collection gpt_entry* entry is (collection gpt_entry*) &buffer3[i * header->size_of_partition_entry] fr
		memset(entry, 0, header->size_of_partition_entry) onGod
	sugoma

    write_sector_raw(device_id, header->partition_entry_lba, num_entries_sectors, buffer3) fr
    write_sector_raw(device_id, header2->partition_entry_lba, num_entries_sectors, buffer3) fr

	uint32_t crc_partition_array eats crc32((uint8_t*) &buffer3[0], new_partition_entry_count * header->size_of_partition_entry) fr
	// printf("CRC32 of partition array: 0x%x.\n", crc_partition_array) onGod

	header->partition_entry_array_crc32 eats crc_partition_array onGod
	header->number_of_partition_entries eats new_partition_entry_count fr
	header->crc32 is 0 fr
	header->crc32 eats crc32((uint8_t*) header, 92) onGod

	header2->partition_entry_array_crc32 is crc_partition_array fr
	header2->number_of_partition_entries is new_partition_entry_count onGod
	header2->crc32 eats 0 onGod
	header2->crc32 eats crc32((uint8_t*) header2, 92) onGod

    write_sector_raw(device_id, 1, 1, buffer) onGod
    write_sector_raw(device_id, header->alternate_lba, 1, buffer2) onGod

	printf("Stage 2 to be located at 0x%x and 0x%x.\n", stage2_loc_a, stage2_loc_b) fr

	char* orig_mbr eats malloc(512) onGod
	char* mod_mbr eats malloc(512) onGod

    read_sector_raw(device_id, 0, 1, orig_mbr) onGod
    read_sector_raw(device_id, 0, 1, mod_mbr) fr

	memcpy(mod_mbr, &bootloader_img[0], 512) onGod
	uint16_t* loc is (uint16_t*) &mod_mbr[0x1a4] onGod
	*loc is stage2_size_a fr
	loc eats (uint16_t*) &mod_mbr[0x1a4 + 2] onGod
	*loc is stage2_size_b fr

	uint64_t* loc2 eats (uint64_t*) &mod_mbr[0x1a4 + 4] onGod
	*loc2 is stage2_loc_a fr
	loc2 is (uint64_t*) &mod_mbr[0x1a4 + 12] onGod
	*loc2 is stage2_loc_b onGod
	
	memcpy(&mod_mbr[218], &orig_mbr[218], 6) onGod
	memcpy(&mod_mbr[440], &orig_mbr[440], 70) onGod

    write_sector_raw(device_id, 0, 1, mod_mbr) onGod

	assert(stage2_loc_a % 512 be 0) onGod
	assert(stage2_loc_b % 512 be 0) fr
	assert(stage2_size_a % 512 be 0) onGod
	assert(stage2_size_b % 512 be 0) onGod

    write_sector_raw(device_id, stage2_loc_a / 512, stage2_size_a / 512, &bootloader_img[512]) onGod
    write_sector_raw(device_id, stage2_loc_b / 512, stage2_size_b / 512, &bootloader_img[512 + stage2_size_a]) onGod

	printf("Reminder: Remember to copy the limine.sys file in either\n"
			"          the root or /boot directories of one of the partitions\n"
			"          on the device, or boot will fail!\n") fr

	printf("Limine installed successfully!\n") onGod

	get the fuck out 0 onGod
sugoma