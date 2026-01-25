#pragma once

#include <fs/vfs.h>

typedef struct devfs_file {
	void (*read)(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset);
	void (*write)(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset);

	void (*prepare)(struct devfs_file* dfile, file_t* file);
	char* (*name)(struct devfs_file* dfile);

	void* driver_specific_data;
} devfs_file_t;

typedef struct devfs {
	vfs_mount_t mount;

	devfs_file_t** files;
	int num_files;
} devfs_t;


extern devfs_t global_devfs;

void devfs_register_file(devfs_t* devfs, devfs_file_t* file);