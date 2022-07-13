#include <interpreter.h>

#include <stdio.h>
#include <stdlib.h>

// int __interpreter_pow(int num_args, int args[]) {
// 	if (num_args != 2) {
// 		printf("Error: pow() takes exactly 2 arguments\n");
// 		abort();
// 	}

// 	return (int) pow(args[0], args[1]);
// }

// int __interpreter_function(char* name, int num_args, int args[]) {
// 	interpreter_function_t functions[] = {
// 		{
// 			.name = "pow",
// 			.function = __interpreter_pow
// 		}
// 	};

// 	for (int i = 0; i < sizeof(functions) / sizeof(interpreter_function_t); i++) {
// 		if (strcmp(name, functions[i].name) == 0) {
// 			return functions[i].function(num_args, args);
// 		}
// 	}

// 	printf("Unknown function: %s\n", name);
// 	abort();
// }

int eval(parser_node_t* node) {
	switch (node->type) {
		case number_node:
			return (int) node->value;		

		case add_node:
			return eval(node->node_a) + eval(node->node_b);
		
		case substract_node:
			return eval(node->node_a) - eval(node->node_b);
		
		case multiply_node:
			return eval(node->node_a) * eval(node->node_b);
		
		case divide_node:
			return eval(node->node_a) / eval(node->node_b);
		
		case modulo_node:
			return eval(node->node_a) % eval(node->node_b);
		
		// case fcall_node:
		// 	{
		// 		printf("fcall_node 1\n");
		// 		parser_function_call_t* function_call = node->value;

		// 		int args[function_call->num_args];
		// 		printf("fcall_node 2\n");

		// 		int idx = 0;

		// 		dynamic_array_t array = function_call->args;
		// 		dynamic_array_iterator_t iterator;
		// 		dynamic_array_iterator_init(&iterator, &array);
		// 		printf("fcall_node 3\n");

		// 		while (dynamic_array_iterator_has_next(&iterator)) {
		// 			printf("fcall_node 4\n");
		// 			parser_node_t* arg = dynamic_array_iterator_get_next(&iterator);
		// 			printf("fcall_node 5\n");
		// 			args[idx++] = eval(arg);
		// 		}

		// 		return __interpreter_function(function_call->name, function_call->num_args, args);
		// 	}
		
		default:
			{
				printf("Unknown node type: %d\n", node->type);
				abort();
			}
	}
}