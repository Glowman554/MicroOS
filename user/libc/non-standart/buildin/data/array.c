#include <non-standart/buildin/data/array.h>

#include <stdlib.h>
#include <string.h>


void* array_create(size_t element_size, size_t initial_capacity) {
    array_header_t* header = malloc(sizeof(array_header_t) + element_size * initial_capacity);
    header->element_size = element_size;
    header->length = 0;
    header->capacity = initial_capacity;
    return &header[1];
}


void array_destroy(void* arr) {
    array_header_t* header = &((array_header_t*)arr)[-1];
    free(header);
}

array_header_t* array_grow(array_header_t* header) {
    size_t new_capacity = header->capacity * 2;
    array_header_t* new_header = realloc(header, sizeof(array_header_t) + header->element_size * new_capacity);
    new_header->capacity = new_capacity;
    return new_header;
}

void* array_push(void* arr, void* element) {
    array_header_t* header = &((array_header_t*)arr)[-1];
    if (header->length >= header->capacity) {
        header = array_grow(header);
        arr = &header[1];
    }
    char* dst = (char*)arr + header->element_size * header->length;
    memcpy(dst, element, header->element_size);
    header->length++;
    return arr;
}

size_t array_length(void* arr) {
    array_header_t* header = (array_header_t*)arr - 1;
    return header->length;
}