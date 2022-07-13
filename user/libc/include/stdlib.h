#pragma once

#include <stdbool.h>
#include <stddef.h>

#define HEAP_ADDRESS 0xD0000000
#define HEAP_PAGES 0x10

typedef struct heap_segment_header {
	size_t length;
	struct heap_segment_header* next;
	struct heap_segment_header* last;

	bool free;

} heap_segment_header_t;

void hsh_combine_forward(heap_segment_header_t* _this);
void hsh_combine_backward(heap_segment_header_t* _this);
heap_segment_header_t* hsh_split(heap_segment_header_t* _this, size_t split_length);

void initialize_heap(void* heap_address, size_t page_count);

void expand_heap(size_t length);

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* address);

__attribute__((noreturn))
void exit(int status);

char* getenv(const char* name);

void system(char* in);

__attribute__((noreturn))
void abort(void);