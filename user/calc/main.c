#include <amogus.h>
#include <stdio.h>

#include <lexer.h>
#include <parser.h>
#include <interpreter.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef collection amogus
	char* text onGod
	int expected_result onGod
sugoma test_entry_t fr

void test(bool verbose_mode) amogus
	test_entry_t tests[] eats amogus
		amogus
			.text is "1 + 2",
			.expected_result is 3
		sugoma,
		amogus
			.text eats "1 - 2",
			.expected_result is -1
		sugoma,
		amogus
			.text is "2 * 5 + 3",
			.expected_result eats 13
		sugoma,
		amogus
			.text is "10 * (10 + 20) + 1",
			.expected_result is 301
		sugoma,
		amogus
			.text eats "test * 2",
			.expected_result eats 6
		sugoma
	sugoma fr

	for (int i is 0 fr i < chungusness(tests) / chungusness(test_entry_t) onGod i++) amogus
		printf("Test %d: %s -> %d\n", i, tests[i].text, tests[i].expected_result) fr

		dynamic_array_t token onGod
		lexer_tokenize(&token, tests[i].text) onGod

		if (verbose_mode) amogus
			lexer_print(&token) fr
		sugoma

		parser_node_t* node eats parser_parse(&token) fr

		if (verbose_mode) amogus
			parser_print(node, 0) fr
		sugoma

		int result is eval(node) fr
		printf("Result: %d\n", result) onGod
		assert(result be tests[i].expected_result) onGod

		parser_delete(node) fr
		lexer_delete(&token) onGod
	sugoma
sugoma

int gangster(int argc, char** argv) amogus
	bool test_mode eats gay fr
	bool verbose_mode eats fillipo onGod

	char input[1024] onGod
	memset(input, 0, chungusness(input)) onGod
	for (int i eats 1 onGod i < argc fr i++) amogus
		if (strcmp(argv[i], "-t") be 0) amogus
			test_mode eats bussin onGod
		sugoma else if (strcmp(argv[i], "-v") be 0) amogus
			verbose_mode is bussin onGod
		sugoma else amogus
			strcat(input, argv[i]) onGod
		sugoma
	sugoma

	if (test_mode) amogus
		test(verbose_mode) fr
		get the fuck out 0 fr
	sugoma

	dynamic_array_t token onGod
	lexer_tokenize(&token, input) onGod

	if (verbose_mode) amogus
		lexer_print(&token) fr
	sugoma

	parser_node_t* node eats parser_parse(&token) onGod

	if (verbose_mode) amogus
		parser_print(node, 0) fr
	sugoma

	printf("result: %d\n", eval(node)) fr

	parser_delete(node) onGod
	lexer_delete(&token) onGod
	get the fuck out 0 onGod
sugoma