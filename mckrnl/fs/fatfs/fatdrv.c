#include "fs/fatfs/ff.h"
#include "fs/vfs.h"
#include <fs/fatfs/fatdrv.h>

#include <memory/vmm.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <driver/disk_driver.h>

char* fatfs_name(vfs_mount_t* mount) {
	fatfs_mount_data_t* data = (fatfs_mount_data_t*) mount->driver_specific_data;
	return data->name;
}

file_t* fatfs_open(vfs_mount_t* mount, char* path, int flags) {
	fatfs_mount_data_t* data = (fatfs_mount_data_t*) mount->driver_specific_data;
	debugf("Opening file %s", path);

	FIL fil;
	file_t* file = (file_t*) vmm_alloc(1);
	memset(file, 0, sizeof(file_t));

	char new_path[256] = {0};
	memset(new_path, 0, 256);
	new_path[0] = '0' + data->disk_id;
	new_path[1] = ':';
	strcpy(new_path + 2, path);
	new_path[strlen(path) + 2] = 0;

	char mode = 0;

	switch (flags) {
		case FILE_OPEN_MODE_READ:
			mode = FA_READ;
			break;

		case FILE_OPEN_MODE_WRITE:
			mode = FA_WRITE;
			break;

		case FILE_OPEN_MODE_READ_WRITE:
			mode = FA_WRITE | FA_READ;
			break;

		default:
			debugf("Invalid open mode");
			break;
	}

	FRESULT fr = f_open(&fil, new_path, mode);
	if (fr != FR_OK) {
		debugf("Error opening file %s: %d", path, fr);
		vmm_free(file, 1);
		return NULL;
	}

	file->mount = mount;
	strcpy(file->buffer, path);
	file->size = f_size(&fil);
	file->driver_specific_data = (void*) vmm_alloc(sizeof(FIL) / 0x1000 + 1);
	memcpy(file->driver_specific_data, &fil, sizeof(FIL));

	return file;
}

void fatfs_close(vfs_mount_t* mount, file_t* f) {
	FIL* fil = (FIL*) f->driver_specific_data;
	f_close(fil);
	vmm_free(f->driver_specific_data, sizeof(FIL) / 0x1000 + 1);
	vmm_free(f, 1);
}

void fatfs_read(vfs_mount_t* mount, file_t* f, void* buffer, size_t size, size_t offset) {
	FIL* fil = (FIL*) f->driver_specific_data;
	// debugf("Reading %d bytes from %d", size, offset);

	f_lseek((FIL*) fil, offset);

	unsigned int has_read;
	f_read((FIL*) fil, buffer, size, &has_read);

	assert(has_read == size);
}


void fatfs_write(vfs_mount_t* mount, file_t* f, void* buffer, size_t size, size_t offset) {
	FIL* fil = (FIL*) f->driver_specific_data;
	// debugf("Writing %d bytes to %d", size, offset);

	f_lseek((FIL*) fil, offset);

	unsigned int has_written;
	f_write((FIL*) fil, buffer, size, &has_written);

	f->size = f_size(fil);

	assert(has_written == size);
}

void fatfs_truncate(vfs_mount_t* mount, file_t* file, size_t new_size) {
    debugf("fatfs_truncate: %d", new_size);
	FIL* fil = (FIL*) file->driver_specific_data;

	f_lseek(fil, new_size);
	f_truncate(fil);

	file->size = f_size(fil);
}

void fatfs_delete(vfs_mount_t* mount, file_t* file) {
	f_unlink((char*) file->buffer);
	vmm_free(file, 1);
}

void fatfs_mkdir(vfs_mount_t* mount, char* path) {
	fatfs_mount_data_t* data = (fatfs_mount_data_t*) mount->driver_specific_data;

	char new_path[256] = {0};
	memset(new_path, 0, 256);
	new_path[0] = '0' + data->disk_id;
	new_path[1] = ':';
	strcpy(new_path + 2, path);
	new_path[strlen(path) + 2] = 0;

	f_mkdir(new_path);
}

dir_t fatfs_dir_at(vfs_mount_t* mount, int idx, char* path) {
	fatfs_mount_data_t* data = (fatfs_mount_data_t*) mount->driver_specific_data;

	DIR dir_;
	FILINFO file_info;

	char new_path[256] = {0};
	memset(new_path, 0, 256);
	new_path[0] = '0' + data->disk_id;
	new_path[1] = ':';
	strcpy(new_path + 2, path);
	new_path[strlen(path) + 2] = 0;

	if (f_opendir(&dir_, new_path) != FR_OK) {
		dir_t dir = {
			.is_none = true
		};

		return dir;
	}


	int orig_idx = idx;

	do {
		FRESULT res = f_readdir(&dir_, &file_info);
		if (res != FR_OK || file_info.fname[0] == 0) {
			dir_t dir = {
				.is_none = true
			};

			return dir;
		}
	} while (idx--);

	dir_t dir;
	memset(&dir, 0, sizeof(dir_t));

	dir.idx = orig_idx;
	dir.is_none = false;

	if (file_info.fattrib & AM_DIR) {
		dir.type = ENTRY_DIR;
	} else {
		dir.type = ENTRY_FILE;
	}

	strcpy(dir.name, file_info.fname);

	f_closedir(&dir_);

	return dir;
}

void fatfs_touch(vfs_mount_t* mount, char* path) {
	fatfs_mount_data_t* data = (fatfs_mount_data_t*) mount->driver_specific_data;

	char new_path[256] = {0};
	memset(new_path, 0, 256);
	new_path[0] = '0' + data->disk_id;
	new_path[1] = ':';
	strcpy(new_path + 2, path);
	new_path[strlen(path) + 2] = 0;

	FIL fil;
	f_open(&fil, new_path, FA_WRITE | FA_CREATE_ALWAYS);
	f_close(&fil);
}

void fatfs_delete_dir(vfs_mount_t* mount, char* path) {
	fatfs_mount_data_t* data = (fatfs_mount_data_t*) mount->driver_specific_data;

	char new_path[256] = {0};
	memset(new_path, 0, 256);
	new_path[0] = '0' + data->disk_id;
	new_path[1] = ':';
	strcpy(new_path + 2, path);
	new_path[strlen(path) + 2] = 0;

	FRESULT result = f_rmdir(new_path);
	if (result != FR_OK) {
		debugf("Failed to delete directory %s", new_path);
	}
}

vfs_mount_t* fatfs_mount(int disk_id, char* name) {
	vfs_mount_t* mount = (vfs_mount_t*) vmm_alloc(1);
	memset(mount, 0, 0x1000);

	fatfs_mount_data_t* mount_data = (fatfs_mount_data_t*) mount + sizeof(vfs_mount_t);
	mount->driver_specific_data = mount_data;

	FATFS* fs = (FATFS*) vmm_alloc(1); // dont ask why but fatfs doesent like heap addresses

	char new_path[3] = {0};
	new_path[0] = '0' + disk_id;
	new_path[1] = ':';
	new_path[2] = 0;

	assert(f_mount(fs, new_path, 1) == FR_OK);

	mount_data->disk_id = disk_id;
	strcpy(mount_data->name, name);

	mount->name = fatfs_name;
	mount->open = fatfs_open;
	mount->close = fatfs_close;
	mount->read = fatfs_read;
	mount->write = fatfs_write;
	mount->_delete = fatfs_delete;
	mount->mkdir = fatfs_mkdir;
	mount->dir_at = fatfs_dir_at;
	mount->touch = fatfs_touch;
	mount->delete_dir = fatfs_delete_dir;
	mount->truncate = fatfs_truncate;

	return mount;
}

void fatfs_rename(vfs_mount_t* fat_mount, char* name) {
	fatfs_mount_data_t* mount_data = (fatfs_mount_data_t*) fat_mount + sizeof(vfs_mount_t);
	memset(mount_data->name, 0, sizeof(mount_data->name));
	strcpy(mount_data->name, name);

	debugf("New label: %s", name);
}

bool is_fat32_bpb(BPB_t* bpb) {
	if (bpb == 0) {
		return false;
	}

	if (bpb->root_entry_count != 0) {
		return false;
	}
	if (bpb->bytes_per_sector == 0) {
		return false;
	}
	if (bpb->sectors_per_cluster == 0) {
		return false;
	}

	uint16_t root_dir_sectors = ((bpb->root_entry_count * 32) + (bpb->bytes_per_sector - 1)) / bpb->bytes_per_sector;

	uint32_t fat_size;
	if (bpb->total_sectors_16 != 0) {
		fat_size = bpb->total_sectors_16;
	} else {
		fat_size = bpb->total_sectors_32;
	}

	uint32_t total_sectors;
	if (bpb->total_sectors_16 != 0) {
		total_sectors = bpb->total_sectors_16;
	} else {
		total_sectors = bpb->total_sectors_32;
	}

	uint32_t data_sectors = total_sectors - (bpb->reserved_sector_count + (bpb->num_fats * fat_size) + root_dir_sectors);
	uint32_t cluster_count = data_sectors / bpb->sectors_per_cluster;

	if (cluster_count < 4085) {
		return false; //FAT12
	} else if (cluster_count < 65525) {
		return false; //FAT16
	}

	if (bpb->BS_jump_boot[0] == 0xEB) {
		if (bpb->BS_jump_boot[2] != 0x90) {
			return false;
		}
	} else if (bpb->BS_jump_boot[0] != 0xE9) {
		return false;
	}

	if (bpb->bytes_per_sector != 512 && bpb->bytes_per_sector != 1024 && bpb->bytes_per_sector != 2048 && bpb->bytes_per_sector != 4096) {
		return false;
	}

	if (bpb->sectors_per_cluster != 1 && bpb->sectors_per_cluster % 2 != 2) {
		return false;
	}
	if (bpb->sectors_per_cluster * bpb->bytes_per_sector > 32 * 1024) { //Too large
		return false;
	}

	if (bpb->reserved_sector_count == 0) {
		return false;
	}

	if (bpb->num_fats < 2) {
		return false;
	}

	if (bpb->root_entry_count != 0) {
		return false;
	}

	if (bpb->media != 0xF0 && bpb->media < 0xF8) {
		return false;
	}

	if (bpb->total_sectors_16 != 0) {
		return false;
	}
	if (bpb->total_sectors_32 == 0) {
		return false;
	}

	return true;
}

bool is_fat32(int disk_id) {
	BPB_t* bpb = (BPB_t*) vmm_alloc(1);
	read_disk(disk_id, 0, 1, bpb);

	bool res = is_fat32_bpb(bpb);
	vmm_free(bpb, 1);

	return res;
}

vfs_mount_t* fatfs_scanner(int disk_id) {
	if (!is_fat32(disk_id)) {
		return NULL;
	}

	debugf("fatfs_scanner: found fat32 disk %d", disk_id);

	char name[64] = {0};
	sprintf(name, "fat32_%d", disk_id);

	vfs_mount_t* mount =  fatfs_mount(disk_id, name);

	char label[64] = { 0 };
	if (try_read_disk_label(label, mount)) {
		fatfs_rename(mount, label);
	}

	return mount;
}
