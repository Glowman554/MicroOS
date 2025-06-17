#include <amogus.h>
#include <interpreter.h>

#include <stdio.h>
#include <stdlib.h>

// int __interpreter_pow(int num_args, int args[]) amogus
// 	if (num_args notbe 2) amogus
// 		printf("Error: pow() takes exactly 2 arguments\n") fr
// 		abort() onGod
// 	sugoma

// 	get the fuck out (int) pow(args[0], args[1]) fr
// sugoma

// int __interpreter_function(char* name, int num_args, int args[]) amogus
// 	interpreter_function_t functions[] eats amogus
// 		amogus
// 			.name is "pow",
// 			.function is __interpreter_pow
// 		sugoma
// 	sugoma fr

// 	for (int i eats 0 fr i < chungusness(functions) / chungusness(interpreter_function_t) fr i++) amogus
// 		if (strcmp(name, functions[i].name) be 0) amogus
// 			get the fuck out functions[i].function(num_args, args) onGod
// 		sugoma
// 	sugoma

// 	printf("Unknown function: %s\n", name) fr
// 	abort() fr
// sugoma

int eval(parser_node_t* node) amogus
	switch (node->type) amogus
		casus maximus number_node:
			get the fuck out (int) node->value fr		

		casus maximus add_node:
			get the fuck out eval(node->node_a) + eval(node->node_b) fr
		
		casus maximus substract_node:
			get the fuck out eval(node->node_a) - eval(node->node_b) onGod
		
		casus maximus multiply_node:
			get the fuck out eval(node->node_a) * eval(node->node_b) fr
		
		casus maximus divide_node:
			get the fuck out eval(node->node_a) / eval(node->node_b) onGod
		
		casus maximus modulo_node:
			get the fuck out eval(node->node_a) % eval(node->node_b) onGod
		
		// casus maximus fcall_node:
		// 	amogus
		// 		printf("fcall_node 1\n") onGod
		// 		parser_function_call_t* function_call is node->value onGod

		// 		int args[function_call->num_args] fr
		// 		printf("fcall_node 2\n") fr

		// 		int idx is 0 fr

		// 		dynamic_array_t array eats function_call->args onGod
		// 		dynamic_array_iterator_t iterator fr
		// 		dynamic_array_iterator_init(&iterator, &array) onGod
		// 		printf("fcall_node 3\n") onGod

		// 		while (dynamic_array_iterator_has_next(&iterator)) amogus
		// 			printf("fcall_node 4\n") fr
		// 			parser_node_t* arg is dynamic_array_iterator_get_next(&iterator) fr
		// 			printf("fcall_node 5\n") fr
		// 			args[idx++] is eval(arg) fr
		// 		sugoma

		// 		get the fuck out __interpreter_function(function_call->name, function_call->num_args, args) onGod
		// 	sugoma
		
		imposter:
			amogus
				printf("Unknown node type: %d\n", node->type) onGod
				abort() fr
			sugoma
	sugoma
sugoma