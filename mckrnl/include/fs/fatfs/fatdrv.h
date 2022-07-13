#pragma once

#include <fs/vfs.h>
#include <fs/fatfs/ff.h>

typedef struct fatfs_mount_data {
	FATFS* fs;
	int disk_id;
	char name[64];
} fatfs_mount_data_t;

typedef struct BPB {
	uint8_t BS_jump_boot[3];
	uint8_t BS_oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sector_count;
	uint8_t num_fats;
	uint16_t root_entry_count;
	uint16_t total_sectors_16;
	uint8_t media;
	uint16_t fat_size_16;
	uint16_t sectors_per_track;
	uint16_t number_of_heads;
	uint32_t hidden_sectors;
	uint32_t total_sectors_32;
} __attribute__ ((__packed__)) BPB_t;

vfs_mount_t* fatfs_mount(int disk_id, char* name);
vfs_mount_t* fatfs_scanner(int disk_id);