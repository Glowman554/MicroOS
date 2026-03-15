#include <module.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <config.h>
#include <memory/heap.h>

#include <fs/devfs.h>

char* system_buffer = NULL;
int system_buffer_size = 0;

void system_buffer_append(const char* str) {
    int str_len = strlen(str);
    char* new_buffer = krealloc(system_buffer, system_buffer_size + str_len + 1);
    if (!new_buffer) {
        return;
    }
    system_buffer = new_buffer;
    memcpy(system_buffer + system_buffer_size, str, str_len + 1);
    system_buffer_size += str_len;
}

void system_file_read(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    if (offset >= system_buffer_size) {
        return;
    }
    if (offset + size > system_buffer_size) {
        size = system_buffer_size - offset;
    }
    memcpy(buf, system_buffer + offset, size);
}

void system_file_prepare(struct devfs_file* dfile, file_t* file) {
    file->size = system_buffer_size;
}

char* system_file_name(devfs_file_t* file) {
	return "system";
}

devfs_file_t system_file = {
	.read = system_file_read,
	.prepare = system_file_prepare,
	.name = system_file_name
};

void main() {
}

void stage_mount() {
    char buf[1024] = { 0 };

    char* cursor = buf;
    cursor += sprintf(cursor, "mckrnl - Abi: %d\n", ABI_VERSION);

    cursor += sprintf(cursor, "Loaded modules:\n");
    for (int i = 0; i < loaded_module_count; i++) {
        cursor += sprintf(cursor, " - %s\n", loaded_modules[i]->name);
    }

    system_buffer_append(buf);

    devfs_register_file(&global_devfs, &system_file);
}

define_module("system", main, NULL, stage_mount);