#pragma once

#include <stddef.h>
#include <stdbool.h>

#define MB(x) ((x) * 1024 * 1024)

typedef struct heap_segment_header {
	size_t length;
	struct heap_segment_header* next;
	struct heap_segment_header* last;

	bool free;

} heap_segment_header_t;

void* kmalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* address);
void* kcalloc(size_t count, size_t size);

void initialize_heap(size_t page_count);