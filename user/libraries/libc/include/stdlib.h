#pragma once

#include <stddef.h>
#include <config.h>


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

__attribute__((noreturn))
void exit(int status);

char* getenv(const char* name);

int system(const char* in);

__attribute__((noreturn))
void abort(void);

int atoi(const char *str);

long strtol(const char *nptr, char **endptr, int base);