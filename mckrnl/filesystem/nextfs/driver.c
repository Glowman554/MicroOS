#include <nextfs.h>
#include <driver/disk_driver.h>
#include <memory/vmm.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

char* nextfs_name(vfs_mount_t* mount) {
	nextfs_mount_data_t* data = (nextfs_mount_data_t*) mount->driver_specific_data;
	return data->header.label;
}

file_t* nextfs_open(vfs_mount_t* mount, char* path, int flags) {
	nextfs_mount_data_t* data = (nextfs_mount_data_t*) mount->driver_specific_data;

    char file[0xff] = { 0 };
    while (*(path = copy_until('/', path, file)));
    debugf("file: %s", file);

    for (int i = 1; i < data->header.file_header_index; i++) {
		if(strcmp(data->file_header[i].name, file) == 0) {

            file_t* file = (file_t*) vmm_alloc(1);
            memset(file, 0, sizeof(file_t));

            file->mount = mount;
            file->size = (data->file_header[i].length + 1) * 512;
            file->driver_specific_data = vmm_alloc(TO_PAGES(file->size));

            read_disk(data->disk_id, data->file_header[i].start_sector, data->file_header[i].length + 1, file->driver_specific_data);

            return file;
		}
	}

    return NULL;
}

void nextfs_close(vfs_mount_t* mount, file_t* f) {
    vmm_free(f->driver_specific_data, TO_PAGES(f->size));
	vmm_free(f, 1);
}

void nextfs_read(vfs_mount_t* mount, file_t* f, void* buffer, size_t size, size_t offset) {
    memcpy(buffer, &((char*) f->driver_specific_data)[offset], size);
}


dir_t nextfs_dir_at(vfs_mount_t* mount, int idx, char* path) {
	nextfs_mount_data_t* data = (nextfs_mount_data_t*) mount->driver_specific_data;
    if ((idx + 1) >= data->header.file_header_index) {
        return (dir_t) {
            .is_none = true
        };
    }

    dir_t dir = {
        .idx = idx,
        .is_none = false,
        .type = ENTRY_FILE
    };
    strcpy(dir.name, data->file_header[idx + 1].name);

    return dir;
}

vfs_mount_t* nextfs_mount(int disk_id) {
    vfs_mount_t* mount = (vfs_mount_t*) vmm_alloc(1);
	memset(mount, 0, 0x1000);

    assert((sizeof(vfs_mount_t) + sizeof(nextfs_mount_data_t)) <= 0x1000);

	nextfs_mount_data_t* mount_data = (nextfs_mount_data_t*) &mount[1];
	mount->driver_specific_data = mount_data;

    read_disk(disk_id, 17, 1, &mount_data->header);
	for (int i = 0; i < 4; i++) {
        read_disk(disk_id, 18 + i, 1, (uint8_t*) &mount_data->file_header + 512 * i);
	}

	mount_data->disk_id = disk_id;

	mount->name = nextfs_name;
	mount->open = nextfs_open;
	mount->close = nextfs_close;
	mount->read = nextfs_read;
	mount->dir_at = nextfs_dir_at;

	return mount;
}


vfs_mount_t* nextfs_scanner(int disk_id) {
    struct nextfs_header header;
    read_disk(disk_id, 17, 1, &header);

    if (header.magic == MAGIC) {
        debugf("found nextfs partition '%s' on disk %d", header.label, disk_id);

        return nextfs_mount(disk_id);
    }

    return NULL;
}