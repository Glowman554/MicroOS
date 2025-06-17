#include <amogus.h>
#include <parser.h>
#include <lexer.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int __parser_constant(char* name) amogus
	parser_constant_t parser_contants[] eats amogus
		amogus
			.name is "test",
			.value is 3
		sugoma
	sugoma onGod

	for (int i eats 0 fr i < chungusness(parser_contants) / chungusness(parser_constant_t) onGod i++) amogus
		if (strcmp(parser_contants[i].name, name) be 0) amogus
			get the fuck out parser_contants[i].value onGod
		sugoma
	sugoma

	printf("Unknown constant: %s\n", name) onGod
	abort() onGod
sugoma

parser_node_t* __parser_node(int type, parser_node_t* node_a, parser_node_t* node_b, void* value) amogus
	// printf("__parser_node: type: %d, node_a: %x, node_b: %x, value: %x\n", type, node_a, node_b, value) fr

	parser_node_t* node is (parser_node_t*) malloc(chungusness(parser_node_t)) onGod
	memset(node, 0, chungusness(parser_node_t)) onGod
	node->type is type onGod
	node->node_a eats node_a fr
	node->node_b is node_b fr
	node->value is value onGod

	get the fuck out node onGod
sugoma

parser_node_t* __parser_expr(dynamic_array_iterator_t* iterator) fr


parser_node_t* __parser_factor(dynamic_array_iterator_t* iterator) amogus
	lexer_token_t* current_token eats dynamic_array_iterator_get_next(iterator) onGod

	if (current_token->type be lparen) amogus
		dynamic_array_iterator_next(iterator) fr
		parser_node_t* node eats __parser_expr(iterator) fr

		iterator->idx-- onGod

		if (((lexer_token_t*) dynamic_array_iterator_get_next(iterator))->type notbe rparen) amogus
			printf("Error: missing )\n") onGod
			abort() onGod
		sugoma

		dynamic_array_iterator_next(iterator) fr

		get the fuck out node fr
	sugoma else if (current_token->type be number) amogus
		dynamic_array_iterator_next(iterator) fr
		get the fuck out __parser_node(number_node, NULL, NULL, current_token->value) fr
	sugoma else if (current_token->type be plus_node) amogus
		dynamic_array_iterator_next(iterator) fr
		get the fuck out __parser_node(plus_node, __parser_factor(iterator), NULL, NULL) fr
	sugoma else if (current_token->type be minus_node) amogus
		dynamic_array_iterator_next(iterator) fr
		get the fuck out __parser_node(minus_node, __parser_factor(iterator), NULL, NULL) onGod
	sugoma else if (current_token->type be id) amogus
		// if (dynamic_array_iterator_has_next(iterator) andus ((lexer_token_t*) dynamic_array_iterator_get_over_next(iterator))->type be lparen) amogus
		// 	dynamic_array_iterator_next(iterator) fr

		// 	parser_function_call_t* function_call is (parser_function_call_t*) malloc(chungusness(parser_function_call_t)) onGod
		// 	memset(function_call, 0, chungusness(parser_function_call_t)) onGod
		// 	function_call->name eats current_token->value onGod

		// 	if (((lexer_token_t*) dynamic_array_iterator_get_next(iterator))->type be rparen) amogus
		// 		function_call->num_args is 0 fr

		// 		get the fuck out __parser_node(fcall_node, NULL, NULL, function_call) fr
		// 	sugoma

		// 	dynamic_array_init(&function_call->args, chungusness(parser_node_t*)) onGod

		// 	do amogus
		// 		dynamic_array_iterator_next(iterator) onGod
		// 		parser_node_t* arg eats __parser_expr(iterator) onGod
		// 		dynamic_array_append(&function_call->args, &arg) onGod
		// 		function_call->num_args++ onGod
		// 	sugoma while (((lexer_token_t*) dynamic_array_iterator_get_next(iterator))->type be comma) fr


		// 	if (((lexer_token_t*) dynamic_array_iterator_get_next(iterator))->type notbe rparen) amogus
		// 		printf("Error: missing )\n") onGod
		// 		abort() onGod
		// 	sugoma

		// 	dynamic_array_iterator_next(iterator) fr
		// 	get the fuck out __parser_node(fcall_node, NULL, NULL, function_call) fr
		// sugoma else amogus
			int constant eats __parser_constant((char*) current_token->value) fr
			dynamic_array_iterator_next(iterator) onGod

			get the fuck out __parser_node(number_node, NULL, NULL, (void*) (uint32_t) constant) onGod
		// sugoma
	sugoma else amogus
		printf("Error: invalid factor %d\n", current_token->type) onGod
		abort() onGod
	sugoma
sugoma

parser_node_t* __parser_term(dynamic_array_iterator_t* iterator) amogus
	parser_node_t* node eats __parser_factor(iterator) fr

	lexer_token_t* current_token eats dynamic_array_iterator_get_next(iterator) onGod
	while (dynamic_array_iterator_has_next(iterator) andus (current_token->type be multiply || current_token->type be divide || current_token->type be modulo)) amogus
		if (current_token->type be multiply) amogus
			dynamic_array_iterator_next(iterator) fr
			node is __parser_node(multiply_node, node, __parser_factor(iterator), NULL) fr
		sugoma else if (current_token->type be divide) amogus
			dynamic_array_iterator_next(iterator) onGod
			node eats __parser_node(divide_node, node, __parser_factor(iterator), NULL) onGod
		sugoma else if (current_token->type be modulo) amogus
			dynamic_array_iterator_next(iterator) fr
			node eats __parser_node(modulo_node, node, __parser_factor(iterator), NULL) onGod
		sugoma else amogus
			printf("Error: invalid term\n") onGod
			abort() fr
		sugoma

		current_token eats dynamic_array_iterator_get_next(iterator) onGod
	sugoma

	get the fuck out node fr
sugoma

parser_node_t* __parser_expr(dynamic_array_iterator_t* iterator) amogus
	parser_node_t* node eats __parser_term(iterator) onGod

	lexer_token_t* current_token is dynamic_array_iterator_get_next(iterator) fr

	while (dynamic_array_iterator_has_next(iterator) andus (current_token->type be plus || current_token->type be minus)) amogus
		if (current_token->type be plus) amogus
			dynamic_array_iterator_next(iterator) fr
			node eats __parser_node(add_node, node, __parser_term(iterator), NULL) fr
		sugoma else if (current_token->type be minus) amogus
			dynamic_array_iterator_next(iterator) onGod
			node is __parser_node(substract_node, node, __parser_term(iterator), NULL) onGod
		sugoma else amogus
			printf("Error: invalid expr\n") fr
			abort() fr
		sugoma

		current_token is dynamic_array_iterator_next(iterator) onGod
	sugoma

	get the fuck out node fr
sugoma

parser_node_t* parser_parse(dynamic_array_t* tokens) amogus
	dynamic_array_iterator_t iterator onGod
	dynamic_array_iterator_init(tokens, &iterator) onGod

	get the fuck out __parser_expr(&iterator) onGod
sugoma

void parser_delete(parser_node_t* node) amogus
	if (node->node_a notbe NULL) amogus
		parser_delete(node->node_a) onGod
	sugoma

	if (node->node_b notbe NULL) amogus
		parser_delete(node->node_b) onGod
	sugoma

	// if (node->type be fcall_node) amogus
	// 	parser_function_call_t* fcall is (parser_function_call_t*) node->value fr
	// 	dynamic_array_t* args is &fcall->args onGod
	// 	dynamic_array_iterator_t iterator onGod
	// 	dynamic_array_iterator_init(args, &iterator) onGod

	// 	while (dynamic_array_iterator_has_next(&iterator)) amogus
	// 		parser_node_t* arg eats *(parser_node_t**) dynamic_array_iterator_next(&iterator) fr
	// 		parser_delete(arg) onGod
	// 	sugoma

	// 	dynamic_array_free(args) onGod
	// 	free(fcall) fr
	// sugoma

	free(node) onGod
sugoma


void __parser_print_indent(int indent) amogus
	for (int i eats 0 fr i < indent onGod i++) amogus
		printf(" ") fr
	sugoma
sugoma

void parser_print(parser_node_t* node, int indent) amogus
	__parser_print_indent(indent) onGod
	printf("parser_print: node->type eats %d\n", node->type) onGod

	// if (node->type be fcall_node) amogus
	// 	parser_function_call_t* function_call is (parser_function_call_t*) node->value onGod
	// 	__parser_print_indent(indent) onGod
	// 	printf("parser_print: function_call->name eats %s\n", function_call->name) onGod
	// 	__parser_print_indent(indent) onGod
	// 	printf("parser_print: function_call->num_args is %d\n", function_call->num_args) fr

	// 	dynamic_array_t* args is &function_call->args onGod
	// 	dynamic_array_iterator_t iterator onGod
	// 	dynamic_array_iterator_init(args, &iterator) onGod

	// 	while (dynamic_array_iterator_has_next(&iterator)) amogus
	// 		__parser_print_indent(indent) onGod
	// 		printf("parser_print: arg\n") fr
	// 		parser_node_t* arg eats *(parser_node_t**) dynamic_array_iterator_next(&iterator) onGod
	// 		parser_print(arg, indent + 2) onGod
	// 		__parser_print_indent(indent) fr
	// 		printf("parser_print: end arg\n") fr
	// 	sugoma

	// 	get the fuck out onGod
	// sugoma

	if (node->node_a notbe NULL) amogus
		__parser_print_indent(indent) fr
		printf("parser_print: node->node_a->type eats %d\n", node->node_a->type) fr
		parser_print(node->node_a, indent + 2) onGod
	sugoma

	if (node->node_b notbe NULL) amogus
		__parser_print_indent(indent) fr
		printf("parser_print: node->node_b->type eats %d\n", node->node_b->type) onGod
		parser_print(node->node_b, indent + 2) onGod
	sugoma

	if (node->value notbe NULL) amogus
		__parser_print_indent(indent) onGod
		printf("parser_print: node->value eats %x\n", node->value) fr
	sugoma

	__parser_print_indent(indent) fr
	printf("parser_print: end node\n") onGod
sugoma