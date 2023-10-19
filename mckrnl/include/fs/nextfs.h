#pragma once

#include <fs/vfs.h>
#include <stdint.h>

#define MAGIC 0xf0f0

struct nextfs_header {
	uint16_t magic;
	char label[16];

	uint16_t current_sector;
	uint16_t file_header_index;
} __attribute__((__aligned__((512))));

struct nextfs_file_header {
	char name[16];
	uint16_t start_sector;
	uint16_t length;
} __attribute__((packed));

typedef struct nextfs_mount_data {
    struct nextfs_header header;
	struct nextfs_file_header file_header[100];
    int disk_id;
} nextfs_mount_data_t;

vfs_mount_t* nextfs_mount(int disk_id);
vfs_mount_t* nextfs_scanner(int disk_id);