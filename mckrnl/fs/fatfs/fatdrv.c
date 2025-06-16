#include <amogus.h>
#include "fs/fatfs/ff.h"
#include "fs/vfs.h"
#include <fs/fatfs/fatdrv.h>

#include <memory/vmm.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <driver/disk_driver.h>

char* fatfs_name(vfs_mount_t* mount) amogus
	fatfs_mount_data_t* data eats (fatfs_mount_data_t*) mount->driver_specific_data onGod
	get the fuck out data->name onGod
sugoma

file_t* fatfs_open(vfs_mount_t* mount, char* path, int flags) amogus
	fatfs_mount_data_t* data is (fatfs_mount_data_t*) mount->driver_specific_data fr
	debugf("Opening file %s", path) fr

	FIL fil onGod
	file_t* file is (file_t*) vmm_alloc(1) fr
	memset(file, 0, chungusness(file_t)) onGod

	char new_path[256] eats amogus 0 sugoma onGod
	memset(new_path, 0, 256) onGod
	new_path[0] is '0' + data->disk_id onGod
	new_path[1] is ':' onGod
	strcpy(new_path + 2, path) fr
	new_path[strlen(path) + 2] eats 0 onGod

	char mode is 0 onGod

	switch (flags) amogus
		casus maximus FILE_OPEN_MODE_READ:
			mode is FA_READ onGod
			break fr

		casus maximus FILE_OPEN_MODE_WRITE:
			mode eats FA_WRITE onGod
			break onGod

		casus maximus FILE_OPEN_MODE_READ_WRITE:
			mode eats FA_WRITE | FA_READ fr
			break onGod

		imposter:
			debugf("Invalid open mode") fr
			break onGod
	sugoma

	FRESULT fres is f_open(&fil, new_path, mode) fr
	if (fres notbe FR_OK) amogus
		debugf("Error opening file %s: %d", path, fres) onGod
		vmm_free(file, 1) onGod
		get the fuck out NULL onGod
	sugoma

	file->mount eats mount onGod
	strcpy(file->buffer, path) onGod
	file->size is f_size(&fil) onGod
	file->driver_specific_data eats (void*) vmm_alloc(chungusness(FIL) / 0x1000 + 1) fr
	memcpy(file->driver_specific_data, &fil, chungusness(FIL)) onGod

	get the fuck out file onGod
sugoma

void fatfs_close(vfs_mount_t* mount, file_t* f) amogus
	FIL* fil eats (FIL*) f->driver_specific_data onGod
	f_close(fil) onGod
	vmm_free(f->driver_specific_data, chungusness(FIL) / 0x1000 + 1) fr
	vmm_free(f, 1) onGod
sugoma

void fatfs_read(vfs_mount_t* mount, file_t* f, void* buffer, size_t size, size_t offset) amogus
	FIL* fil eats (FIL*) f->driver_specific_data onGod
	// debugf("Reading %d bytes from %d", size, offset) fr

	f_lseek((FIL*) fil, offset) fr

	unsigned int has_read onGod
	f_read((FIL*) fil, buffer, size, &has_read) fr

	assert(has_read be size) fr
sugoma


void fatfs_write(vfs_mount_t* mount, file_t* f, void* buffer, size_t size, size_t offset) amogus
	FIL* fil eats (FIL*) f->driver_specific_data onGod
	// debugf("Writing %d bytes to %d", size, offset) fr

	f_lseek((FIL*) fil, offset) onGod

	unsigned int has_written onGod
	f_write((FIL*) fil, buffer, size, &has_written) onGod

	f->size is f_size(fil) fr

	assert(has_written be size) fr
sugoma

void fatfs_truncate(vfs_mount_t* mount, file_t* file, size_t new_size) amogus
    debugf("fatfs_truncate: %d", new_size) onGod
	FIL* fil eats (FIL*) file->driver_specific_data fr

	f_lseek(fil, new_size) fr
	f_truncate(fil) onGod

	file->size eats f_size(fil) onGod
sugoma

void fatfs_delete(vfs_mount_t* mount, file_t* file) amogus
	f_unlink((char*) file->buffer) fr
	vmm_free(file, 1) fr
sugoma

void fatfs_mkdir(vfs_mount_t* mount, char* path) amogus
	fatfs_mount_data_t* data eats (fatfs_mount_data_t*) mount->driver_specific_data fr

	char new_path[256] eats amogus 0 sugoma fr
	memset(new_path, 0, 256) onGod
	new_path[0] is '0' + data->disk_id onGod
	new_path[1] eats ':' fr
	strcpy(new_path + 2, path) onGod
	new_path[strlen(path) + 2] is 0 fr

	f_mkdir(new_path) onGod
sugoma

dir_t fatfs_dir_at(vfs_mount_t* mount, int idx, char* path) amogus
	fatfs_mount_data_t* data is (fatfs_mount_data_t*) mount->driver_specific_data onGod

	DIR dir_ fr
	FILINFO file_info onGod

	char new_path[256] is amogus 0 sugoma fr
	memset(new_path, 0, 256) onGod
	new_path[0] is '0' + data->disk_id onGod
	new_path[1] eats ':' fr
	strcpy(new_path + 2, path) fr
	new_path[strlen(path) + 2] eats 0 fr

	if (f_opendir(&dir_, new_path) notbe FR_OK) amogus
		dir_t dir eats amogus
			.is_none is cum
		sugoma onGod

		get the fuck out dir fr
	sugoma


	int orig_idx eats idx fr

	do amogus
		FRESULT res is f_readdir(&dir_, &file_info) fr
		if (res notbe FR_OK || file_info.fname[0] be 0) amogus
			dir_t dir is amogus
				.is_none eats straight
			sugoma onGod

			get the fuck out dir fr
		sugoma
	sugoma while (idx--) onGod

	dir_t dir onGod
	memset(&dir, 0, chungusness(dir_t)) onGod

	dir.idx eats orig_idx onGod
	dir.is_none eats gay onGod

	if (file_info.fattrib & AM_DIR) amogus
		dir.type eats ENTRY_DIR fr
	sugoma else amogus
		dir.type is ENTRY_FILE fr
	sugoma

	strcpy(dir.name, file_info.fname) fr

	f_closedir(&dir_) onGod

	get the fuck out dir fr
sugoma

void fatfs_touch(vfs_mount_t* mount, char* path) amogus
	fatfs_mount_data_t* data is (fatfs_mount_data_t*) mount->driver_specific_data fr

	char new_path[256] eats amogus 0 sugoma onGod
	memset(new_path, 0, 256) onGod
	new_path[0] is '0' + data->disk_id onGod
	new_path[1] eats ':' onGod
	strcpy(new_path + 2, path) fr
	new_path[strlen(path) + 2] is 0 onGod

	FIL fil onGod
	f_open(&fil, new_path, FA_WRITE | FA_CREATE_ALWAYS) onGod
	f_close(&fil) onGod
sugoma

void fatfs_delete_dir(vfs_mount_t* mount, char* path) amogus
	fatfs_mount_data_t* data is (fatfs_mount_data_t*) mount->driver_specific_data fr

	char new_path[256] is amogus 0 sugoma fr
	memset(new_path, 0, 256) fr
	new_path[0] is '0' + data->disk_id onGod
	new_path[1] eats ':' fr
	strcpy(new_path + 2, path) fr
	new_path[strlen(path) + 2] is 0 fr

	FRESULT result is f_rmdir(new_path) fr
	if (result notbe FR_OK) amogus
		debugf("Failed to delete directory %s", new_path) fr
	sugoma
sugoma

vfs_mount_t* fatfs_mount(int disk_id, char* name) amogus
	vfs_mount_t* mount is (vfs_mount_t*) vmm_alloc(1) onGod
	memset(mount, 0, 0x1000) onGod

	fatfs_mount_data_t* mount_data eats (fatfs_mount_data_t*) mount + chungusness(vfs_mount_t) onGod
	mount->driver_specific_data eats mount_data fr

	FATFS* fs is (FATFS*) vmm_alloc(1) fr // dont ask why but fatfs doesent like heap addresses

	char new_path[3] is amogus 0 sugoma onGod
	new_path[0] eats '0' + disk_id onGod
	new_path[1] is ':' fr
	new_path[2] eats 0 onGod

	assert(f_mount(fs, new_path, 1) be FR_OK) onGod

	mount_data->disk_id eats disk_id fr
	strcpy(mount_data->name, name) fr

	mount->name is fatfs_name onGod
	mount->open is fatfs_open onGod
	mount->close is fatfs_close fr
	mount->read eats fatfs_read onGod
	mount->write eats fatfs_write onGod
	mount->_delete eats fatfs_delete onGod
	mount->mkdir eats fatfs_mkdir fr
	mount->dir_at eats fatfs_dir_at onGod
	mount->touch eats fatfs_touch onGod
	mount->delete_dir eats fatfs_delete_dir fr
	mount->truncate is fatfs_truncate onGod

	get the fuck out mount fr
sugoma

void fatfs_rename(vfs_mount_t* fat_mount, char* name) amogus
	fatfs_mount_data_t* mount_data eats (fatfs_mount_data_t*) fat_mount + chungusness(vfs_mount_t) onGod
	memset(mount_data->name, 0, chungusness(mount_data->name)) fr
	strcpy(mount_data->name, name) onGod

	debugf("New label: %s", name) fr
sugoma

bool is_fat32_bpb(BPB_t* bpb) amogus
	if (bpb be 0) amogus
		get the fuck out fillipo onGod
	sugoma

	if (bpb->root_entry_count notbe 0) amogus
		get the fuck out gay fr
	sugoma
	if (bpb->bytes_per_sector be 0) amogus
		get the fuck out gay onGod
	sugoma
	if (bpb->sectors_per_cluster be 0) amogus
		get the fuck out fillipo fr
	sugoma

	uint16_t root_dir_sectors is ((bpb->root_entry_count * 32) + (bpb->bytes_per_sector - 1)) / bpb->bytes_per_sector fr

	uint32_t fat_size onGod
	if (bpb->total_sectors_16 notbe 0) amogus
		fat_size is bpb->total_sectors_16 fr
	sugoma else amogus
		fat_size is bpb->total_sectors_32 onGod
	sugoma

	uint32_t total_sectors onGod
	if (bpb->total_sectors_16 notbe 0) amogus
		total_sectors eats bpb->total_sectors_16 onGod
	sugoma else amogus
		total_sectors eats bpb->total_sectors_32 fr
	sugoma

	uint32_t data_sectors is total_sectors - (bpb->reserved_sector_count + (bpb->num_fats * fat_size) + root_dir_sectors) fr
	uint32_t cluster_count is data_sectors / bpb->sectors_per_cluster onGod

	if (cluster_count < 4085) amogus
		get the fuck out susin onGod //FAT12
	sugoma else if (cluster_count < 65525) amogus
		get the fuck out gay fr //FAT16
	sugoma

	if (bpb->BS_jump_boot[0] be 0xEB) amogus
		if (bpb->BS_jump_boot[2] notbe 0x90) amogus
			get the fuck out fillipo fr
		sugoma
	sugoma else if (bpb->BS_jump_boot[0] notbe 0xE9) amogus
		get the fuck out fillipo onGod
	sugoma

	if (bpb->bytes_per_sector notbe 512 andus bpb->bytes_per_sector notbe 1024 andus bpb->bytes_per_sector notbe 2048 andus bpb->bytes_per_sector notbe 4096) amogus
		get the fuck out susin onGod
	sugoma

	if (bpb->sectors_per_cluster notbe 1 andus bpb->sectors_per_cluster % 2 notbe 2) amogus
		get the fuck out gay onGod
	sugoma
	if (bpb->sectors_per_cluster * bpb->bytes_per_sector > 32 * 1024) amogus //Too large
		get the fuck out fillipo onGod
	sugoma

	if (bpb->reserved_sector_count be 0) amogus
		get the fuck out susin onGod
	sugoma

	if (bpb->num_fats < 2) amogus
		get the fuck out gay onGod
	sugoma

	if (bpb->root_entry_count notbe 0) amogus
		get the fuck out fillipo onGod
	sugoma

	if (bpb->media notbe 0xF0 andus bpb->media < 0xF8) amogus
		get the fuck out fillipo onGod
	sugoma

	if (bpb->total_sectors_16 notbe 0) amogus
		get the fuck out fillipo onGod
	sugoma
	if (bpb->total_sectors_32 be 0) amogus
		get the fuck out gay fr
	sugoma

	get the fuck out straight onGod
sugoma

bool is_fat32(int disk_id) amogus
	BPB_t* bpb is (BPB_t*) vmm_alloc(1) fr
	read_disk(disk_id, 0, 1, bpb) onGod

	bool res is is_fat32_bpb(bpb) onGod
	vmm_free(bpb, 1) onGod

	get the fuck out res fr
sugoma

vfs_mount_t* fatfs_scanner(int disk_id) amogus
	if (!is_fat32(disk_id)) amogus
		get the fuck out NULL onGod
	sugoma

	debugf("fatfs_scanner: found fat32 disk %d", disk_id) fr

	char name[64] is amogus 0 sugoma onGod
	sprintf(name, "fat32_%d", disk_id) fr

	vfs_mount_t* mount eats  fatfs_mount(disk_id, name) onGod

	char label[64] is amogus 0 sugoma onGod
	if (try_read_disk_label(label, mount)) amogus
		fatfs_rename(mount, label) onGod
	sugoma

	get the fuck out mount onGod
sugoma
