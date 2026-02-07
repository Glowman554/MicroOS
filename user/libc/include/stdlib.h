#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <config.h>

#define HEAP_ADDRESS 0xD0000000
#define HEAP_PAGES 0x10

typedef struct heap_segment_header {
	size_t length;
	struct heap_segment_header* next;
	struct heap_segment_header* last;

#ifdef ALLOC_STORE_LOCATION
	const char* file;
	const char* func;
	int line;
#endif

	bool free;

} heap_segment_header_t;

void hsh_combine_forward(heap_segment_header_t* _this);
void hsh_combine_backward(heap_segment_header_t* _this);
heap_segment_header_t* hsh_split(heap_segment_header_t* _this, size_t split_length);

void initialize_heap(void* heap_address, size_t page_count);

void expand_heap(size_t length);

#ifdef ALLOC_STORE_LOCATION
#define malloc(size) malloc_debug(size, __FILE__, __func__, __LINE__)
#define realloc(ptr, size) realloc_debug(ptr, size, __FILE__, __func__, __LINE__)
#define calloc(count, size) calloc_debug(count, size, __FILE__, __func__, __LINE__)

void* malloc_debug(size_t size, const char* file, const char* func, int line);
void* realloc_debug(void* ptr, size_t size, const char* file, const char* func, int line);
void* calloc_debug(size_t count, size_t size, const char* file, const char* func, int line);
#else
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t count, size_t size);
#endif

void free(void* address);

void print_allocations(const char* msg);

__attribute__((noreturn))
void exit(int status);

char* getenv(const char* name);

void system(char* in);

__attribute__((noreturn))
void abort(void);

int atoi(const char *str);

long strtol(const char *nptr, char **endptr, int base);