#include <stdio.h>

#include <lexer.h>
#include <parser.h>
#include <interpreter.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct {
	char* text;
	int expected_result;
} test_entry_t;

void test(bool verbose_mode) {
	test_entry_t tests[] = {
		{
			.text = "1 + 2",
			.expected_result = 3
		},
		{
			.text = "1 - 2",
			.expected_result = -1
		},
		{
			.text = "2 * 5 + 3",
			.expected_result = 13
		},
		{
			.text = "10 * (10 + 20) + 1",
			.expected_result = 301
		},
		{
			.text = "test * 2",
			.expected_result = 6
		}
	};

	for (int i = 0; i < sizeof(tests) / sizeof(test_entry_t); i++) {
		printf("Test %d: %s -> %d\n", i, tests[i].text, tests[i].expected_result);

		dynamic_array_t token;
		lexer_tokenize(&token, tests[i].text);

		if (verbose_mode) {
			lexer_print(&token);
		}

		parser_node_t* node = parser_parse(&token);

		if (verbose_mode) {
			parser_print(node, 0);
		}

		int result = eval(node);
		printf("Result: %d\n", result);
		assert(result == tests[i].expected_result);

		parser_delete(node);
		lexer_delete(&token);
	}
}

int main(int argc, char** argv) {
	bool test_mode = false;
	bool verbose_mode = false;

	char input[1024];
	memset(input, 0, sizeof(input));
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-t") == 0) {
			test_mode = true;
		} else if (strcmp(argv[i], "-v") == 0) {
			verbose_mode = true;
		} else {
			strcat(input, argv[i]);
		}
	}

	if (test_mode) {
		test(verbose_mode);
		return 0;
	}

	dynamic_array_t token;
	lexer_tokenize(&token, input);

	if (verbose_mode) {
		lexer_print(&token);
	}

	parser_node_t* node = parser_parse(&token);

	if (verbose_mode) {
		parser_print(node, 0);
	}

	printf("result: %d\n", eval(node));

	parser_delete(node);
	lexer_delete(&token);
	return 0;
}