#pragma once

#include <dynamic_array.h>

enum parser_node_type_e {
	number_node,
	add_node,
	substract_node,
	multiply_node,
	divide_node,
	modulo_node,
	plus_node,
	minus_node,
	// fcall_node
};

typedef struct parser_node_t {
	int type;

	struct parser_node_t* node_a;
	struct parser_node_t* node_b;

	void* value;
} parser_node_t;

typedef struct parser_constant_t {
	char* name;
	int value;
} parser_constant_t;

// typedef struct parser_function_call_t {
// 	char* name;
// 	int num_args;
// 	dynamic_array_t args;
// } parser_function_call_t;

void parser_delete(parser_node_t* node);
parser_node_t* parser_parse(dynamic_array_t* tokens);
void parser_print(parser_node_t* node, int indent);