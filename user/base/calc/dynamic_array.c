#include <dynamic_array.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define DA_DEBUG

void dynamic_array_init(dynamic_array_t* array, int sizeof_element) {
	array->length = 0;
	array->sizeof_element = sizeof_element;
	array->data = NULL;

#ifdef DA_DEBUG
	printf("dynamic_array_init: sizeof_element = %d\n", sizeof_element);
#endif
}
void dynamic_array_free(dynamic_array_t* array) {
#ifdef DA_DEBUG
	printf("dynamic_array_free: array->data = %p\n", array->data);
#endif

	if (array->data != NULL) {
		free(array->data);
	}
}

void dynamic_array_append(dynamic_array_t* array, void* element) {
#ifdef DA_DEBUG
	printf("dynamic_array_append: array->length = %d\n", array->length);
#endif

	if (array->data == NULL) {
		array->data = malloc(array->sizeof_element);
	} else {
		array->data = realloc(array->data, (array->length + 1) * array->sizeof_element);
	}
	memcpy(array->data + array->length * array->sizeof_element, element, array->sizeof_element);
	array->length++;
}

void dynamic_array_iterator_init(dynamic_array_t* array, dynamic_array_iterator_t* iterator) {
	iterator->array = array;
	iterator->idx = 0;

#ifdef DA_DEBUG
	printf("dynamic_array_iterator_init: array->length = %d\n", array->length);
#endif
}

void* dynamic_array_iterator_next(dynamic_array_iterator_t* iterator) {
#ifdef DA_DEBUG
	printf("dynamic_array_iterator_next: iterator->idx = %d\n", iterator->idx);
#endif

	if (iterator->idx >= iterator->array->length) {
		return NULL;
	}
	void* element = iterator->array->data + iterator->idx * iterator->array->sizeof_element;
	iterator->idx++;
	return element;
}

void* dynamic_array_iterator_get_next(dynamic_array_iterator_t* iterator) {
	if (iterator->idx >= iterator->array->length) {
		return NULL;
	}

	return iterator->array->data + iterator->idx * iterator->array->sizeof_element;
}

void* dynamic_array_iterator_get_over_next(dynamic_array_iterator_t* iterator) {
	if (iterator->idx + 1 >= iterator->array->length) {
		return NULL;
	}

	return iterator->array->data + (iterator->idx + 1) * iterator->array->sizeof_element;
}

bool dynamic_array_iterator_has_next(dynamic_array_iterator_t* iterator) {
#ifdef DA_DEBUG
	printf("dynamic_array_iterator_has_next: iterator->idx = %d\n", iterator->idx);
#endif

	return iterator->idx < iterator->array->length;
}