#pragma once

#include <parser.h>

typedef struct interpreter_function_t {
	char* name;
	int (*function)(int, int[]);
} interpreter_function_t;

int eval(parser_node_t* node);