#include <fatdrv.h>

#include <memory/heap.h>
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
	debugf(SPAM, "Opening file %s", path);

	FIL fil;
	file_t* file = (file_t*) kmalloc(sizeof(file_t));
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
			debugf(ERROR, "Invalid open mode");
			break;
	}

	FRESULT fr = f_open(&fil, new_path, mode);
	if (fr != FR_OK) {
		debugf(SPAM, "Error opening file %s: %d", path, fr);
		kfree(file);
		return NULL;
	}

	file->mount = mount;
	strcpy(file->buffer, path);
	file->size = f_size(&fil);
	file->driver_specific_data = (void*) kmalloc(sizeof(FIL));
	memcpy(file->driver_specific_data, &fil, sizeof(FIL));

	return file;
}

void fatfs_close(vfs_mount_t* mount, file_t* f) {
	FIL* fil = (FIL*) f->driver_specific_data;
	f_close(fil);
	kfree(f->driver_specific_data);
	kfree(f);
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
    debugf(SPAM, "fatfs_truncate: %d", new_size);
	FIL* fil = (FIL*) file->driver_specific_data;

	f_lseek(fil, new_size);
	f_truncate(fil);

	file->size = f_size(fil);
}

void fatfs_delete(vfs_mount_t* mount, file_t* file) {
	f_unlink((char*) file->buffer);
	kfree(file);
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
		debugf(ERROR, "Failed to delete directory %s", new_path);
	}
}

vfs_mount_t* fatfs_mount(int disk_id, char* name) {
	vfs_mount_t* mount = (vfs_mount_t*) kmalloc(sizeof(vfs_mount_t) + sizeof(fatfs_mount_data_t));
	memset(mount, 0, sizeof(vfs_mount_t) + sizeof(fatfs_mount_data_t));

	fatfs_mount_data_t* mount_data = (fatfs_mount_data_t*) &mount[1];
	mount->driver_specific_data = mount_data;

	FATFS* fs = (FATFS*) kmalloc(sizeof(FATFS));

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
	fatfs_mount_data_t* mount_data = (fatfs_mount_data_t*) &fat_mount[1];
	memset(mount_data->name, 0, sizeof(mount_data->name));
	strcpy(mount_data->name, name);

	debugf(SPAM, "New label: %s", name);
}

static uint32_t read_le32(const uint8_t* ptr) {
	return (uint32_t)ptr[0] | ((uint32_t)ptr[1] << 8) | ((uint32_t)ptr[2] << 16) | ((uint32_t)ptr[3] << 24);
}

bool is_fat32_bpb(BPB_t* bpb) {
	if (bpb == NULL) {
		debugf(ERROR, "BPB is null");
		return false;
	}

	const uint8_t* raw = (const uint8_t*) bpb;
	uint16_t sector_size = bpb->bytes_per_sector;
	uint8_t cluster_size = bpb->sectors_per_cluster;
	uint16_t reserved = bpb->reserved_sector_count;
	uint8_t fats = bpb->num_fats;
	uint16_t root_entries = bpb->root_entry_count;
	uint32_t total_sectors = bpb->total_sectors_32 != 0 ? bpb->total_sectors_32 : bpb->total_sectors_16;
	uint32_t fat_size_16 = bpb->fat_size_16;
	uint32_t fat_size_32 = read_le32(raw + 36);
	uint32_t root_cluster = read_le32(raw + 44);

	if (sector_size == 0) {
		debugf(ERROR, "Bytes per sector is 0");
		return false;
	}
	if ((sector_size & (sector_size - 1)) != 0 || sector_size < 512 || sector_size > 4096) {
		debugf(ERROR, "Invalid FAT32 BPB: unsupported bytes per sector: %d", sector_size);
		return false;
	}

	if (cluster_size == 0 || (cluster_size & (cluster_size - 1)) != 0) {
		debugf(ERROR, "Invalid FAT32 BPB: invalid sectors per cluster: %d", cluster_size);
		return false;
	}

	if ((uint32_t)sector_size * cluster_size > 32 * 1024) {
		debugf(ERROR, "Invalid FAT32 BPB: cluster size is too large: %u bytes", (uint32_t)sector_size * cluster_size);
		return false;
	}

	if (reserved == 0) {
		debugf(ERROR, "Invalid FAT32 BPB: reserved sector count is 0");
		return false;
	}

	if (fats == 0 || fats > 2) {
		debugf(ERROR, "Invalid FAT32 BPB: invalid FAT count: %d", fats);
		return false;
	}

	if (root_entries != 0) {
		debugf(ERROR, "Invalid FAT32 BPB: root entry count must be 0");
		return false;
	}

	if (total_sectors == 0) {
		debugf(ERROR, "Invalid FAT32 BPB: total sectors is 0");
		return false;
	}

	if (fat_size_16 != 0) {
		debugf(ERROR, "Invalid FAT32 BPB: FAT size 16 is not zero (%u)", fat_size_16);
		return false;
	}

	if (fat_size_32 == 0) {
		debugf(ERROR, "Invalid FAT32 BPB: FAT size 32 is 0");
		return false;
	}

	if (root_cluster < 2) {
		debugf(ERROR, "Invalid FAT32 BPB: root cluster is invalid: %u", root_cluster);
		return false;
	}

	uint32_t data_sectors = total_sectors - (reserved + fats * fat_size_32);
	if (data_sectors == 0) {
		debugf(ERROR, "Invalid FAT32 BPB: no data sectors");
		return false;
	}

	uint32_t cluster_count = data_sectors / cluster_size;
	if (cluster_count < 65525) {
		debugf(ERROR, "Cluster count is too small for FAT32: %u", cluster_count);
		return false;
	}

	if (cluster_count > 0x0FFFFFF5U) {
		debugf(ERROR, "Invalid FAT32 BPB: cluster count is too large: %u", cluster_count);
		return false;
	}

	bool has_valid_jump = (bpb->BS_jump_boot[0] == 0xEB || bpb->BS_jump_boot[0] == 0xE9 || bpb->BS_jump_boot[0] == 0xE8);
	if (!has_valid_jump) {
		debugf(ERROR, "Invalid FAT32 BPB: invalid jump instruction 0x%02x", bpb->BS_jump_boot[0]);
		return false;
	}

	bool has_signature = (raw[510] == 0x55 && raw[511] == 0xAA);
	bool has_fat32_label = (memcmp(raw + 82, "FAT32   ", 8) == 0);

	if (!has_signature && !has_fat32_label) {
		debugf(ERROR, "Invalid FAT32 BPB: missing boot signature and FAT32 label");
		return false;
	}

	if (bpb->media != 0xF0 && bpb->media < 0xF8) {
		debugf(ERROR, "Invalid FAT32 BPB: invalid media type: 0x%02x", bpb->media);
		return false;
	}

	return true;
}

bool is_fat32(int disk_id) {
	BPB_t* bpb = (BPB_t*) kmalloc(512);
	read_disk(disk_id, 0, 1, bpb);

	bool res = is_fat32_bpb(bpb);
	kfree(bpb);
	return res;
}

vfs_mount_t* fatfs_scanner(int disk_id) {
	if (!is_fat32(disk_id)) {
		return NULL;
	}

	debugf(SPAM, "fatfs_scanner: found fat32 disk %d", disk_id);

	char name[64] = {0};
	sprintf(name, "fat32_%d", disk_id);

	vfs_mount_t* mount =  fatfs_mount(disk_id, name);

	char label[64] = { 0 };
	if (try_read_disk_label(label, mount)) {
		fatfs_rename(mount, label);
	}

	return mount;
}
