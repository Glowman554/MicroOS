#pragma once

#include <stddef.h>

typedef struct array_header {
    size_t element_size;
    size_t length;
    size_t capacity;
} array_header_t;

void* array_create(size_t element_size, size_t initial_capacity);
void array_destroy(void* arr);
void* array_push(void* arr, void* element);
size_t array_length(void* arr);