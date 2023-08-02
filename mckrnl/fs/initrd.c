#include <fs/initrd.h>

#include <stdio.h>
#include <memory/vmm.h>
#include <utils/string.h>
#include <assert.h>

saf_node_hdr_t* initrd_find(char* path, void* base, saf_node_hdr_t* current) {
	debugf("initrd_find(%s, %x, %x, %s)", path, base, current, current->name);

	while (*path == '/') {
		path++;
	}
	if (*path == 0) {
		return current;
	}

	char buffer[128] = { 0 };
	char* next = copy_until('/', path, buffer);

    assert(current->flags == FLAG_ISFOLDER);
	saf_node_folder_t* folder_node = (saf_node_folder_t*) current;

	for (int i = 0; i < folder_node->num_children; i++) {
        saf_node_hdr_t* child = (saf_node_hdr_t*) ((uint32_t) base + (uint32_t) folder_node->children[i]);
		if (strcasecmp(buffer, child->name) == 0) {
			return initrd_find(next, base, child);
		}
	}

	return NULL;
}


char* initrd_name(vfs_mount_t* mount) {
	return "initrd";
}

file_t* initrd_open(vfs_mount_t* mount, char* path, int flags) {
	debugf("open: %s", path);

	saf_node_hdr_t* file = initrd_find(path, mount->driver_specific_data, (saf_node_hdr_t*) mount->driver_specific_data);
	if (file == NULL) {
		debugf("file %s not found", path);
		return NULL;
	}

	if (file->flags == FLAG_ISFOLDER) {
		debugf("file %s is a folder", path);
		return NULL;
	}

	saf_node_file_t* file_node = (saf_node_file_t*) file;

	file_t* f = (file_t*) vmm_alloc(1);
	f->mount = mount;
	f->size = file_node->size;
	f->driver_specific_data = (void*) ((uint32_t) mount->driver_specific_data + (uint32_t) file_node->addr);

	return f;
}

void initrd_close(vfs_mount_t* mount, file_t* f) {
	vmm_free(f, 1);
}

void initrd_read(vfs_mount_t* mount, file_t* f, void* buffer, size_t size, size_t offset) {
	assert(f->size >= offset + size);

	memcpy(buffer, (void*) ((uint32_t) f->driver_specific_data + offset), size);
}

dir_t initrd_dir_at(vfs_mount_t* mount, int idx, char* path) {
	// debugf("dir_at: %s (%d)", path, idx);
	char path_cpy[strlen(path) + 1];
	strcpy(path_cpy, path);
	path_cpy[strlen(path)] = 0;;

	saf_node_hdr_t* folder = initrd_find(path, mount->driver_specific_data, (saf_node_hdr_t*) mount->driver_specific_data);

	if (folder == NULL) {
		dir_t dir = {
			.is_none = true,
		};
		return dir;
	}

	if (folder->flags != FLAG_ISFOLDER) {
		dir_t dir = {
			.is_none = true,
		};
		return dir;
	}

	saf_node_folder_t* folder_node = (saf_node_folder_t*) folder;

	if (idx > folder_node->num_children - 1) {
		dir_t dir = {
			.is_none = true,
		};
		return dir;
	} else {
		saf_node_hdr_t* child = (saf_node_hdr_t*) ((uint32_t) mount->driver_specific_data + (uint32_t) folder_node->children[idx]);

		dir_t dir;
		memset(&dir, 0, sizeof(dir));

		strcpy(dir.name, child->name);
		dir.idx = idx;
		dir.is_none = false;

		if (child->flags == FLAG_ISFOLDER) {
			dir.type = ENTRY_DIR;
		} else {
			dir.type = ENTRY_FILE;
		}

		return dir;
	}
}

vfs_mount_t* initrd_mount(void* saf_image) {
	vfs_mount_t* mount = (vfs_mount_t*) vmm_alloc(1);
	memset(mount, 0, sizeof(vfs_mount_t));

	mount->driver_specific_data = saf_image;

	mount->name = initrd_name;
	mount->open = initrd_open;
	mount->close = initrd_close;
	mount->read = initrd_read;
	mount->dir_at = initrd_dir_at;

	return mount;
}