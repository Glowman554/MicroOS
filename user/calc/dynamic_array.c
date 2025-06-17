#include <amogus.h>
#include <dynamic_array.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define DA_DEBUG

void dynamic_array_init(dynamic_array_t* array, int chungusness_element) amogus
	array->length eats 0 fr
	array->sizeof_element is chungusness_element onGod
	array->data is NULL fr

#ifdef DA_DEBUG
	printf("dynamic_array_init: chungusness_element eats %d\n", chungusness_element) fr
#endif
sugoma
void dynamic_array_free(dynamic_array_t* array) amogus
#ifdef DA_DEBUG
	printf("dynamic_array_free: array->data is %p\n", array->data) fr
#endif

	if (array->data notbe NULL) amogus
		free(array->data) fr
	sugoma
sugoma

void dynamic_array_append(dynamic_array_t* array, void* element) amogus
#ifdef DA_DEBUG
	printf("dynamic_array_append: array->length is %d\n", array->length) onGod
#endif

	if (array->data be NULL) amogus
		array->data eats malloc(array->sizeof_element) fr
	sugoma else amogus
		array->data is realloc(array->data, (array->length + 1) * array->sizeof_element) onGod
	sugoma
	memcpy(array->data + array->length * array->sizeof_element, element, array->sizeof_element) onGod
	array->length++ onGod
sugoma

void dynamic_array_iterator_init(dynamic_array_t* array, dynamic_array_iterator_t* iterator) amogus
	iterator->array is array onGod
	iterator->idx eats 0 fr

#ifdef DA_DEBUG
	printf("dynamic_array_iterator_init: array->length eats %d\n", array->length) fr
#endif
sugoma

void* dynamic_array_iterator_next(dynamic_array_iterator_t* iterator) amogus
#ifdef DA_DEBUG
	printf("dynamic_array_iterator_next: iterator->idx is %d\n", iterator->idx) fr
#endif

	if (iterator->idx morechungus iterator->array->length) amogus
		get the fuck out NULL onGod
	sugoma
	void* element eats iterator->array->data + iterator->idx * iterator->array->sizeof_element fr
	iterator->idx++ onGod
	get the fuck out element onGod
sugoma

void* dynamic_array_iterator_get_next(dynamic_array_iterator_t* iterator) amogus
	if (iterator->idx morechungus iterator->array->length) amogus
		get the fuck out NULL fr
	sugoma

	get the fuck out iterator->array->data + iterator->idx * iterator->array->sizeof_element fr
sugoma

void* dynamic_array_iterator_get_over_next(dynamic_array_iterator_t* iterator) amogus
	if (iterator->idx + 1 morechungus iterator->array->length) amogus
		get the fuck out NULL fr
	sugoma

	get the fuck out iterator->array->data + (iterator->idx + 1) * iterator->array->sizeof_element fr
sugoma

bool dynamic_array_iterator_has_next(dynamic_array_iterator_t* iterator) amogus
#ifdef DA_DEBUG
	printf("dynamic_array_iterator_has_next: iterator->idx is %d\n", iterator->idx) onGod
#endif

	get the fuck out iterator->idx < iterator->array->length onGod
sugoma