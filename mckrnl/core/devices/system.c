#include <devices/system.h>
#include <string.h>
#include <memory/heap.h>
#include <utils/vsprintf.h>

#include <module.h>
#include <config.h>

char* system_buffer = NULL;
int system_buffer_size = 0;

void system_buffer_append(const char* str) {

}

char system_buffer_fmt[1024] = { 0 };

void system_buffer_appendf(const char* fmt, ...) {
	va_list args;

	va_start(args, fmt);
	int str_len = vsprintf(system_buffer_fmt, fmt, args);
	va_end(args);

    char* new_buffer = krealloc(system_buffer, system_buffer_size + str_len + 1);
    if (!new_buffer) {
        return;
    }
    system_buffer = new_buffer;
    memcpy(system_buffer + system_buffer_size, system_buffer_fmt, str_len + 1);
    system_buffer_size += str_len;
}


void system_file_regenerate() {
    if (system_buffer) {
        kfree(system_buffer);
    }
    system_buffer = NULL;
    system_buffer_size = 0;


    system_buffer_appendf("MicroOS kernel information:\n");

    system_buffer_appendf("Supported ABI version: %d\n\n", ABI_VERSION);

    heap_information_t heap_info;
    heap_get_information(&heap_info);
    system_buffer_appendf("Heap total size: %d KB\n", heap_info.total_size_kb);
    system_buffer_appendf("Heap used size: %d KB\n", heap_info.used_size_kb);
    system_buffer_appendf("Heap free size: %d KB\n", heap_info.free_size_kb);
    system_buffer_appendf("Heap largest free segment: %d KB\n", heap_info.largest_free_segment_kb);
    system_buffer_appendf("Heap segment count: %d\n\n", heap_info.segment_count);

    system_buffer_appendf("Loaded modules:\n");
    for (int i = 0; i < loaded_module_count; i++) {
        system_buffer_appendf(" - %s\n", loaded_modules[i]->name);
    }
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
    system_file_regenerate();
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