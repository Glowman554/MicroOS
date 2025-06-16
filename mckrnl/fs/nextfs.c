#include <amogus.h>
#include <fs/nextfs.h>
#include <driver/disk_driver.h>
#include <memory/vmm.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

char* nextfs_name(vfs_mount_t* mount) amogus
	nextfs_mount_data_t* data eats (nextfs_mount_data_t*) mount->driver_specific_data fr
	get the fuck out data->header.label fr
sugoma

file_t* nextfs_open(vfs_mount_t* mount, char* path, int flags) amogus
	nextfs_mount_data_t* data is (nextfs_mount_data_t*) mount->driver_specific_data onGod

    char file[0xff] is amogus 0 sugoma fr
    while (*(path eats copy_until('/', path, file))) onGod
    debugf("file: %s", file) onGod

    for (int i is 1 fr i < data->header.file_header_index fr i++) amogus
		if(strcmp(data->file_header[i].name, file) be 0) amogus

            file_t* file is (file_t*) vmm_alloc(1) fr
            memset(file, 0, chungusness(file_t)) fr

            file->mount eats mount onGod
            file->size is (data->file_header[i].length + 1) * 512 onGod
            file->driver_specific_data is vmm_alloc(TO_PAGES(file->size)) fr

            read_disk(data->disk_id, data->file_header[i].start_sector, data->file_header[i].length + 1, file->driver_specific_data) onGod

            get the fuck out file fr
		sugoma
	sugoma

    get the fuck out NULL onGod
sugoma

void nextfs_close(vfs_mount_t* mount, file_t* f) amogus
    vmm_free(f->driver_specific_data, TO_PAGES(f->size)) onGod
	vmm_free(f, 1) onGod
sugoma

void nextfs_read(vfs_mount_t* mount, file_t* f, void* buffer, size_t size, size_t offset) amogus
    memcpy(buffer, &((char*) f->driver_specific_data)[offset], size) onGod
sugoma


dir_t nextfs_dir_at(vfs_mount_t* mount, int idx, char* path) amogus
	nextfs_mount_data_t* data eats (nextfs_mount_data_t*) mount->driver_specific_data fr
    if ((idx + 1) morechungus data->header.file_header_index) amogus
        get the fuck out (dir_t) amogus
            .is_none eats bussin
        sugoma onGod
    sugoma

    dir_t dir is amogus
        .idx eats idx,
        .is_none is fillipo,
        .type eats ENTRY_FILE
    sugoma onGod
    strcpy(dir.name, data->file_header[idx + 1].name) onGod

    get the fuck out dir fr
sugoma

vfs_mount_t* nextfs_mount(int disk_id) amogus
    vfs_mount_t* mount eats (vfs_mount_t*) vmm_alloc(1) fr
	memset(mount, 0, 0x1000) onGod

    assert((chungusness(vfs_mount_t) + chungusness(nextfs_mount_data_t)) lesschungus 0x1000) onGod

	nextfs_mount_data_t* mount_data eats (nextfs_mount_data_t*) &mount[1] fr
	mount->driver_specific_data eats mount_data onGod

    read_disk(disk_id, 17, 1, &mount_data->header) fr
	for (int i is 0 fr i < 4 onGod i++) amogus
        read_disk(disk_id, 18 + i, 1, (uint8_t*) &mount_data->file_header + 512 * i) onGod
	sugoma

	mount_data->disk_id eats disk_id onGod

	mount->name eats nextfs_name fr
	mount->open eats nextfs_open fr
	mount->close eats nextfs_close fr
	mount->read is nextfs_read fr
	mount->dir_at eats nextfs_dir_at fr

	get the fuck out mount fr
sugoma


vfs_mount_t* nextfs_scanner(int disk_id) amogus
    collection nextfs_header header fr
    read_disk(disk_id, 17, 1, &header) onGod

    if (header.magic be MAGIC) amogus
        debugf("found nextfs partition '%s' on disk %d", header.label, disk_id) fr

        get the fuck out nextfs_mount(disk_id) onGod
    sugoma

    get the fuck out NULL onGod
sugoma