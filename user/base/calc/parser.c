#include <parser.h>
#include <lexer.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


int __parser_constant(char* name) {
	parser_constant_t parser_contants[] = {
		{
			.name = "test",
			.value = 3
		}
	};

	for (int i = 0; i < sizeof(parser_contants) / sizeof(parser_constant_t); i++) {
		if (strcmp(parser_contants[i].name, name) == 0) {
			return parser_contants[i].value;
		}
	}

	printf("Unknown constant: %s\n", name);
	abort();
}

parser_node_t* __parser_node(int type, parser_node_t* node_a, parser_node_t* node_b, void* value) {
	// printf("__parser_node: type: %d, node_a: %x, node_b: %x, value: %x\n", type, node_a, node_b, value);

	parser_node_t* node = (parser_node_t*) malloc(sizeof(parser_node_t));
	memset(node, 0, sizeof(parser_node_t));
	node->type = type;
	node->node_a = node_a;
	node->node_b = node_b;
	node->value = value;

	return node;
}

parser_node_t* __parser_expr(dynamic_array_iterator_t* iterator);


parser_node_t* __parser_factor(dynamic_array_iterator_t* iterator) {
	lexer_token_t* current_token = dynamic_array_iterator_get_next(iterator);

	if (current_token->type == lparen) {
		dynamic_array_iterator_next(iterator);
		parser_node_t* node = __parser_expr(iterator);

		iterator->idx--;

		if (((lexer_token_t*) dynamic_array_iterator_get_next(iterator))->type != rparen) {
			printf("Error: missing )\n");
			abort();
		}

		dynamic_array_iterator_next(iterator);

		return node;
	} else if (current_token->type == number) {
		dynamic_array_iterator_next(iterator);
		return __parser_node(number_node, NULL, NULL, current_token->value);
	} else if (current_token->type == plus_node) {
		dynamic_array_iterator_next(iterator);
		return __parser_node(plus_node, __parser_factor(iterator), NULL, NULL);
	} else if (current_token->type == minus_node) {
		dynamic_array_iterator_next(iterator);
		return __parser_node(minus_node, __parser_factor(iterator), NULL, NULL);
	} else if (current_token->type == id) {
		// if (dynamic_array_iterator_has_next(iterator) && ((lexer_token_t*) dynamic_array_iterator_get_over_next(iterator))->type == lparen) {
		// 	dynamic_array_iterator_next(iterator);

		// 	parser_function_call_t* function_call = (parser_function_call_t*) malloc(sizeof(parser_function_call_t));
		// 	memset(function_call, 0, sizeof(parser_function_call_t));
		// 	function_call->name = current_token->value;

		// 	if (((lexer_token_t*) dynamic_array_iterator_get_next(iterator))->type == rparen) {
		// 		function_call->num_args = 0;

		// 		return __parser_node(fcall_node, NULL, NULL, function_call);
		// 	}

		// 	dynamic_array_init(&function_call->args, sizeof(parser_node_t*));

		// 	do {
		// 		dynamic_array_iterator_next(iterator);
		// 		parser_node_t* arg = __parser_expr(iterator);
		// 		dynamic_array_append(&function_call->args, &arg);
		// 		function_call->num_args++;
		// 	} while (((lexer_token_t*) dynamic_array_iterator_get_next(iterator))->type == comma);


		// 	if (((lexer_token_t*) dynamic_array_iterator_get_next(iterator))->type != rparen) {
		// 		printf("Error: missing )\n");
		// 		abort();
		// 	}

		// 	dynamic_array_iterator_next(iterator);
		// 	return __parser_node(fcall_node, NULL, NULL, function_call);
		// } else {
			int constant = __parser_constant((char*) current_token->value);
			dynamic_array_iterator_next(iterator);

			return __parser_node(number_node, NULL, NULL, (void*) (uint32_t) constant);
		// }
	} else {
		printf("Error: invalid factor %d\n", current_token->type);
		abort();
	}
}

parser_node_t* __parser_term(dynamic_array_iterator_t* iterator) {
	parser_node_t* node = __parser_factor(iterator);

	lexer_token_t* current_token = dynamic_array_iterator_get_next(iterator);
	while (dynamic_array_iterator_has_next(iterator) && (current_token->type == multiply || current_token->type == divide || current_token->type == modulo)) {
		if (current_token->type == multiply) {
			dynamic_array_iterator_next(iterator);
			node = __parser_node(multiply_node, node, __parser_factor(iterator), NULL);
		} else if (current_token->type == divide) {
			dynamic_array_iterator_next(iterator);
			node = __parser_node(divide_node, node, __parser_factor(iterator), NULL);
		} else if (current_token->type == modulo) {
			dynamic_array_iterator_next(iterator);
			node = __parser_node(modulo_node, node, __parser_factor(iterator), NULL);
		} else {
			printf("Error: invalid term\n");
			abort();
		}

		current_token = dynamic_array_iterator_get_next(iterator);
	}

	return node;
}

parser_node_t* __parser_expr(dynamic_array_iterator_t* iterator) {
	parser_node_t* node = __parser_term(iterator);

	lexer_token_t* current_token = dynamic_array_iterator_get_next(iterator);

	while (dynamic_array_iterator_has_next(iterator) && (current_token->type == plus || current_token->type == minus)) {
		if (current_token->type == plus) {
			dynamic_array_iterator_next(iterator);
			node = __parser_node(add_node, node, __parser_term(iterator), NULL);
		} else if (current_token->type == minus) {
			dynamic_array_iterator_next(iterator);
			node = __parser_node(substract_node, node, __parser_term(iterator), NULL);
		} else {
			printf("Error: invalid expr\n");
			abort();
		}

		current_token = dynamic_array_iterator_next(iterator);
	}

	return node;
}

parser_node_t* parser_parse(dynamic_array_t* tokens) {
	dynamic_array_iterator_t iterator;
	dynamic_array_iterator_init(tokens, &iterator);

	return __parser_expr(&iterator);
}

void parser_delete(parser_node_t* node) {
	if (node->node_a != NULL) {
		parser_delete(node->node_a);
	}

	if (node->node_b != NULL) {
		parser_delete(node->node_b);
	}

	// if (node->type == fcall_node) {
	// 	parser_function_call_t* fcall = (parser_function_call_t*) node->value;
	// 	dynamic_array_t* args = &fcall->args;
	// 	dynamic_array_iterator_t iterator;
	// 	dynamic_array_iterator_init(args, &iterator);

	// 	while (dynamic_array_iterator_has_next(&iterator)) {
	// 		parser_node_t* arg = *(parser_node_t**) dynamic_array_iterator_next(&iterator);
	// 		parser_delete(arg);
	// 	}

	// 	dynamic_array_free(args);
	// 	free(fcall);
	// }

	free(node);
}


void __parser_print_indent(int indent) {
	for (int i = 0; i < indent; i++) {
		printf(" ");
	}
}

void parser_print(parser_node_t* node, int indent) {
	__parser_print_indent(indent);
	printf("parser_print: node->type = %d\n", node->type);

	// if (node->type == fcall_node) {
	// 	parser_function_call_t* function_call = (parser_function_call_t*) node->value;
	// 	__parser_print_indent(indent);
	// 	printf("parser_print: function_call->name = %s\n", function_call->name);
	// 	__parser_print_indent(indent);
	// 	printf("parser_print: function_call->num_args = %d\n", function_call->num_args);

	// 	dynamic_array_t* args = &function_call->args;
	// 	dynamic_array_iterator_t iterator;
	// 	dynamic_array_iterator_init(args, &iterator);

	// 	while (dynamic_array_iterator_has_next(&iterator)) {
	// 		__parser_print_indent(indent);
	// 		printf("parser_print: arg\n");
	// 		parser_node_t* arg = *(parser_node_t**) dynamic_array_iterator_next(&iterator);
	// 		parser_print(arg, indent + 2);
	// 		__parser_print_indent(indent);
	// 		printf("parser_print: end arg\n");
	// 	}

	// 	return;
	// }

	if (node->node_a != NULL) {
		__parser_print_indent(indent);
		printf("parser_print: node->node_a->type = %d\n", node->node_a->type);
		parser_print(node->node_a, indent + 2);
	}

	if (node->node_b != NULL) {
		__parser_print_indent(indent);
		printf("parser_print: node->node_b->type = %d\n", node->node_b->type);
		parser_print(node->node_b, indent + 2);
	}

	if (node->value != NULL) {
		__parser_print_indent(indent);
		printf("parser_print: node->value = %x\n", node->value);
	}

	__parser_print_indent(indent);
	printf("parser_print: end node\n");
}