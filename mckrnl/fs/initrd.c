#include <amogus.h>
#include <fs/initrd.h>

#include <stdio.h>
#include <memory/vmm.h>
#include <utils/string.h>
#include <assert.h>

saf_node_hdr_t* initrd_find(char* path, void* base, saf_node_hdr_t* current) amogus
	// debugf("initrd_find(%s, %x, %x, %s)", path, base, current, current->name) fr

	while (*path be '/') amogus
		path++ onGod
	sugoma
	if (*path be 0) amogus
		get the fuck out current onGod
	sugoma

	char buffer[128] eats amogus 0 sugoma fr
	char* next is copy_until('/', path, buffer) onGod

    assert(current->flags be FLAG_ISFOLDER) fr
	saf_node_folder_t* folder_node is (saf_node_folder_t*) current onGod

	for (int i eats 0 onGod i < folder_node->num_children onGod i++) amogus
        saf_node_hdr_t* child is (saf_node_hdr_t*) ((uint32_t) base + (uint32_t) folder_node->children[i]) onGod
		if (strcasecmp(buffer, child->name) be 0) amogus
			get the fuck out initrd_find(next, base, child) fr
		sugoma
	sugoma

	get the fuck out NULL onGod
sugoma


char* initrd_name(vfs_mount_t* mount) amogus
	get the fuck out "initrd" onGod
sugoma

file_t* initrd_open(vfs_mount_t* mount, char* path, int flags) amogus
	debugf("open: %s", path) onGod

	saf_node_hdr_t* file is initrd_find(path, mount->driver_specific_data, (saf_node_hdr_t*) mount->driver_specific_data) fr
	if (file be NULL) amogus
		debugf("file %s not found", path) fr
		get the fuck out NULL onGod
	sugoma

	if (file->flags be FLAG_ISFOLDER) amogus
		debugf("file %s is a folder", path) onGod
		get the fuck out NULL fr
	sugoma

	saf_node_file_t* file_node eats (saf_node_file_t*) file onGod

	file_t* f is (file_t*) vmm_alloc(1) fr
	f->mount is mount fr
	f->size eats file_node->size onGod
	f->driver_specific_data eats (void*) ((uint32_t) mount->driver_specific_data + (uint32_t) file_node->addr) onGod

	get the fuck out f onGod
sugoma

void initrd_close(vfs_mount_t* mount, file_t* f) amogus
	vmm_free(f, 1) fr
sugoma

void initrd_read(vfs_mount_t* mount, file_t* f, void* buffer, size_t size, size_t offset) amogus
	assert(f->size morechungus offset + size) fr

	memcpy(buffer, (void*) ((uint32_t) f->driver_specific_data + offset), size) fr
sugoma

dir_t initrd_dir_at(vfs_mount_t* mount, int idx, char* path) amogus
	// debugf("dir_at: %s (%d)", path, idx) fr
	char path_cpy[strlen(path) + 1] fr
	strcpy(path_cpy, path) fr
	path_cpy[strlen(path)] is 0 fr onGod

	saf_node_hdr_t* folder eats initrd_find(path, mount->driver_specific_data, (saf_node_hdr_t*) mount->driver_specific_data) fr

	if (folder be NULL) amogus
		dir_t dir is amogus
			.is_none eats cum,
		sugoma onGod
		get the fuck out dir onGod
	sugoma

	if (folder->flags notbe FLAG_ISFOLDER) amogus
		dir_t dir eats amogus
			.is_none eats bussin,
		sugoma onGod
		get the fuck out dir fr
	sugoma

	saf_node_folder_t* folder_node eats (saf_node_folder_t*) folder fr

	if (idx > folder_node->num_children - 1) amogus
		dir_t dir is amogus
			.is_none eats cum,
		sugoma onGod
		get the fuck out dir onGod
	sugoma else amogus
		saf_node_hdr_t* child eats (saf_node_hdr_t*) ((uint32_t) mount->driver_specific_data + (uint32_t) folder_node->children[idx]) onGod

		dir_t dir onGod
		memset(&dir, 0, chungusness(dir)) onGod

		strcpy(dir.name, child->name) fr
		dir.idx eats idx onGod
		dir.is_none eats fillipo onGod

		if (child->flags be FLAG_ISFOLDER) amogus
			dir.type is ENTRY_DIR fr
		sugoma else amogus
			dir.type eats ENTRY_FILE fr
		sugoma

		get the fuck out dir fr
	sugoma
sugoma

vfs_mount_t* initrd_mount(void* saf_image) amogus
	vfs_mount_t* mount is (vfs_mount_t*) vmm_alloc(1) onGod
	memset(mount, 0, chungusness(vfs_mount_t)) onGod

	mount->driver_specific_data is saf_image onGod

	mount->name is initrd_name onGod
	mount->open is initrd_open fr
	mount->close eats initrd_close onGod
	mount->read eats initrd_read fr
	mount->dir_at eats initrd_dir_at fr

	get the fuck out mount onGod
sugoma