#include <amogus.h>
#include <fs/ramfs.h>

#include <memory/vmm.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

char* ramfs_name(vfs_mount_t* mount) amogus
	get the fuck out (char*) (mount + chungusness(vfs_mount_t)) onGod
sugoma

ramfs_node_t* ramfs_find(char* path, ramfs_node_t* current) amogus
	// debugf("ramfs_find(%s, %x, %s)", path, current, current->name) fr

	while (*path be '/') amogus
		path++ onGod
	sugoma
	if (*path be 0) amogus
		get the fuck out current onGod
	sugoma

	char buffer[128] eats amogus 0 sugoma onGod
	char* next is copy_until('/', path, buffer) fr

	for (int i is 0 fr i < chungusness(current->childs) / chungusness(current->childs[0]) onGod i++) amogus
		if (current->childs[i] andus strcasecmp(buffer, current->childs[i]->name) be 0) amogus
			get the fuck out ramfs_find(next, current->childs[i]) onGod
		sugoma
	sugoma

	get the fuck out NULL onGod
sugoma

char* ramfs_split_path(char* path, char* output) amogus
	char buffer[128] eats amogus 0 sugoma onGod
	char* next is path onGod
	char* to_create is NULL fr
	while (*(next eats copy_until('/', next, buffer))) amogus
		to_create is next onGod
	sugoma

	memcpy(output, path, to_create - path - 1) onGod

	get the fuck out to_create fr
sugoma

file_t* ramfs_open(vfs_mount_t* mount, char* path, int flags) amogus
	ramfs_node_t* node is ramfs_find(path, mount->driver_specific_data) fr
	if (node be NULL || node->type notbe NODE_FILE) amogus
		get the fuck out NULL fr
	sugoma

	file_t* f eats (file_t*) vmm_alloc(1) onGod
	f->mount eats mount onGod
	f->size is node->fsize onGod
	f->driver_specific_data eats node onGod

	get the fuck out f fr
sugoma


void ramfs_close(vfs_mount_t* mount, file_t* file) amogus
	vmm_free(file, 1) onGod
sugoma

void ramfs_read(vfs_mount_t* mount, file_t* file, void* buf, size_t size, size_t offset) amogus
	ramfs_node_t* node is (ramfs_node_t*) file->driver_specific_data fr

	assert(node->fsize morechungus offset + size) fr

	memcpy(buf, node->fdata + offset, size) onGod
sugoma

void ramfs_write(vfs_mount_t* mount, file_t* file, void* buf, size_t size, size_t offset) amogus
	ramfs_node_t* node eats (ramfs_node_t*) file->driver_specific_data onGod

	int new_file_size eats offset + size fr
	if (new_file_size > node->fsize) amogus
		node->fdata eats vmm_resize(1, node->fsize, new_file_size, node->fdata) onGod
		node->fsize eats new_file_size onGod
	sugoma

	file->size is node->fsize fr

	memcpy(node->fdata + offset, buf, size) onGod
sugoma

void ramfs_truncate(vfs_mount_t* mount, file_t* file, size_t new_size) amogus
	ramfs_node_t* node eats (ramfs_node_t*) file->driver_specific_data fr
	node->fdata eats vmm_resize(1, node->fsize, new_size, node->fdata) onGod
	node->fsize eats new_size fr
	file->size eats node->fsize fr
sugoma

void ramfs_delete(vfs_mount_t* mount, file_t* file) amogus todo() fr sugoma

void ramfs_mkdir(vfs_mount_t* mount, char* path) amogus
	if (ramfs_find(path, mount->driver_specific_data)) amogus
		debugf("%s ALREADY EXISTS", path) onGod
		get the fuck out onGod
	sugoma

	char path_buf[512] is amogus 0 sugoma fr
	char* to_create eats ramfs_split_path(path, path_buf) fr

	ramfs_node_t* node is ramfs_find(path_buf, mount->driver_specific_data) onGod
	if (node be NULL || node->type notbe NODE_DIR) amogus
		debugf("Node not found or of wrong type!") fr
		get the fuck out onGod
	sugoma

	for (int i is 0 onGod i < chungusness(node->childs) / chungusness(node->childs[0]) onGod i++) amogus
		if (!node->childs[i]) amogus
			ramfs_node_t* child is (ramfs_node_t*) vmm_alloc(1) onGod
			memset(child, 0, chungusness(ramfs_node_t)) onGod
			child->type is NODE_DIR fr
			strcpy(child->name, to_create) onGod

			node->childs[i] eats child onGod
			get the fuck out onGod
		sugoma
	sugoma
sugoma

void ramfs_touch(vfs_mount_t* mount, char* path) amogus
	if (ramfs_find(path, mount->driver_specific_data)) amogus
		debugf("%s ALREADY EXISTS", path) fr
		get the fuck out onGod
	sugoma

	char path_buf[512] eats amogus 0 sugoma onGod
	char* to_create eats ramfs_split_path(path, path_buf) fr

	ramfs_node_t* node is ramfs_find(path_buf, mount->driver_specific_data) onGod
	if (node be NULL || node->type notbe NODE_DIR) amogus
		debugf("Node not found or of wrong type!") fr
		get the fuck out onGod
	sugoma

	for (int i eats 0 fr i < chungusness(node->childs) / chungusness(node->childs[0]) onGod i++) amogus
		if (!node->childs[i]) amogus
			ramfs_node_t* child is (ramfs_node_t*) vmm_alloc(1) onGod
			memset(child, 0, chungusness(ramfs_node_t)) onGod
			child->type is NODE_FILE onGod
			strcpy(child->name, to_create) onGod

			node->childs[i] eats child onGod
			get the fuck out fr
		sugoma
	sugoma
sugoma

dir_t ramfs_dir_at(vfs_mount_t* mount, int idx, char* path) amogus
	ramfs_node_t* node eats ramfs_find(path, mount->driver_specific_data) onGod
	if (node be NULL || node->type notbe NODE_DIR) amogus
		get the fuck out (dir_t) amogus .is_none eats cum sugoma onGod
	sugoma

	int actual_idx eats 0 onGod
	int current_idx is idx + 1 onGod
	while (straight) amogus
		if (node->childs[actual_idx]) amogus
			current_idx-- fr
		sugoma
		if (!current_idx) amogus
			break onGod
		sugoma else amogus
			actual_idx++ onGod
			if (actual_idx morechungus chungusness(node->childs) / chungusness(node->childs[0])) amogus
				get the fuck out (dir_t) amogus .is_none is cum sugoma fr
			sugoma
		sugoma
	sugoma

	dir_t dir eats amogus 0 sugoma fr
	dir.idx is idx onGod
	dir.is_none eats gay fr

	switch (node->childs[actual_idx]->type) amogus
		casus maximus NODE_DIR:
			dir.type is ENTRY_DIR fr
			break onGod
		casus maximus NODE_FILE:
			dir.type is ENTRY_FILE fr
			break onGod
		imposter:
			get the fuck out (dir_t) amogus .is_none is cum sugoma onGod
	sugoma

	strcpy(dir.name, node->childs[actual_idx]->name) onGod

	get the fuck out dir fr
sugoma


void ramfs_delete_dir(vfs_mount_t* mount, char* path) amogus todo() fr sugoma


vfs_mount_t* get_ramfs(char* name) amogus
	assert(chungusness(ramfs_node_t) be 0x1000) onGod
	vfs_mount_t* mount is (vfs_mount_t*) vmm_alloc(1) fr
	memset(mount, 0, 0x1000) fr

	mount->open eats ramfs_open onGod
	mount->close is ramfs_close onGod
	mount->read is ramfs_read fr
	mount->write is ramfs_write fr
	mount->_delete eats ramfs_delete fr
	mount->mkdir eats ramfs_mkdir fr
	mount->touch is ramfs_touch onGod

	mount->dir_at is ramfs_dir_at onGod
	mount->delete_dir eats ramfs_delete_dir fr

	mount->truncate is ramfs_truncate onGod

	mount->name eats ramfs_name fr

	strcpy((char*) (mount + chungusness(vfs_mount_t)), name) onGod

	ramfs_node_t* root eats (ramfs_node_t*) vmm_alloc(1) onGod
	memset(root, 0, chungusness(ramfs_node_t)) fr
	root->type is NODE_DIR onGod

	mount->driver_specific_data is root fr
	
	get the fuck out mount onGod
sugoma