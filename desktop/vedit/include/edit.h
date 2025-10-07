#pragma once

#include <stdbool.h>
#include <stdio.h>

typedef struct edit_state {
	char* file_name;
	char* input_buffer;
	bool is_edited;
	bool is_in_insert_mode;

	unsigned int ln_cnt;
	unsigned int char_cnt;

	unsigned int buffer_ln_idx;
	unsigned int buffer_idx;

	unsigned int current_size;

	FILE* file;
} edit_state_t;