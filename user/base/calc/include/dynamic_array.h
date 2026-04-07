#pragma once

#include <stdbool.h>

typedef struct {
	void* data;
	int sizeof_element;
	int length;
} dynamic_array_t;

typedef struct {
	dynamic_array_t* array;
	int idx;
} dynamic_array_iterator_t;

void dynamic_array_init(dynamic_array_t* array, int sizeof_element);
void dynamic_array_free(dynamic_array_t* array);

void dynamic_array_append(dynamic_array_t* array, void* element);

void dynamic_array_iterator_init(dynamic_array_t* array, dynamic_array_iterator_t* iterator);
void* dynamic_array_iterator_next(dynamic_array_iterator_t* iterator);
void* dynamic_array_iterator_get_next(dynamic_array_iterator_t* iterator);
void* dynamic_array_iterator_get_over_next(dynamic_array_iterator_t* iterator);
bool dynamic_array_iterator_has_next(dynamic_array_iterator_t* iterator);