#include <lexer.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void lexer_delete(dynamic_array_t* token) {
	dynamic_array_iterator_t iterator;
	dynamic_array_iterator_init(token, &iterator);
	while (dynamic_array_iterator_has_next(&iterator)) {
		lexer_token_t* token = (lexer_token_t*) dynamic_array_iterator_next(&iterator);

		switch (token->type) {
			case id:
				{
					free(token->value);
				}
				break;
			
			default:
				break;
		}
	}

	dynamic_array_free(token);
}

int _lexer_parse_number(dynamic_array_t* token, char* input) {
	int idx = 0;
	int number_system_base = 10;

	if (input[0] == '0') {
		if (input[1] == 'x') {
			number_system_base = 16;
			idx = 2;
		} else if (input[1] == 'b') {
			number_system_base = 2;
			idx = 2;
		}
	}

	int _number = 0;

	while (input[idx] != '\0') {
		if (input[idx] >= '0' && input[idx] <= '9') {
			_number = _number * number_system_base + (input[idx] - '0');
		} else if (input[idx] >= 'a' && input[idx] <= 'f') {
			_number = _number * number_system_base + (input[idx] - 'a' + 10);
		} else if (input[idx] >= 'A' && input[idx] <= 'F') {
			_number = _number * number_system_base + (input[idx] - 'A' + 10);
		} else {
			break;
		}

		idx++;
	}

	// printf("_lexer_parse_number: number = %d\n", _number);

	dynamic_array_append(token, (void*) &(lexer_token_t) { .type = number, .value = (void*) (uint32_t) _number });

	return idx - 1;
}

void lexer_tokenize(dynamic_array_t* token, char* input) {
	dynamic_array_init(token, sizeof(lexer_token_t));
	int idx = 0;
	char current = input[idx];

	do {

		switch (current) {
			case ' ':
			case '\t':
			case '\n':
				break;
			
			case '+':
				{
					dynamic_array_append(token, (void*) &(lexer_token_t) { .type = plus });
				}
				break;
			
			case '-':
				{
					dynamic_array_append(token, (void*) &(lexer_token_t) { .type = minus });
				}
				break;
			
			case '*':
				{
					dynamic_array_append(token, (void*) &(lexer_token_t) { .type = multiply });
				}
				break;
			
			case '/':
				{
					dynamic_array_append(token, (void*) &(lexer_token_t) { .type = divide });
				}
				break;
			
			case '%':
				{
					dynamic_array_append(token, (void*) &(lexer_token_t) { .type = modulo });
				}
				break;

			case '(':
				{
					dynamic_array_append(token, (void*) &(lexer_token_t) { .type = lparen });
				}
				break;

			case ')':
				{
					dynamic_array_append(token, (void*) &(lexer_token_t) { .type = rparen });
				}
				break;
			
			case ',':
				{
					dynamic_array_append(token, (void*) &(lexer_token_t) { .type = comma });
				}
				break;
			
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					idx += _lexer_parse_number(token, &input[idx]);
				}
				break;
			
			
			default:
				{
					if ((current >= 'a' && current <= 'z') || (current >= 'A' && current <= 'Z')) {
						int start_offset = idx;
						idx++;

						while (input[idx] != '\0' && ((input[idx] >= 'a' && input[idx] <= 'z') || (input[idx] >= 'A' && input[idx] <= 'Z'))) {
							idx++;
						}

						char* _id = (char*) malloc(sizeof(char) * (idx - start_offset + 1));
						memcpy(_id, &input[start_offset], sizeof(char) * (idx - start_offset));
						_id[idx - start_offset] = '\0';

						idx--;

						dynamic_array_append(token, (void*) &(lexer_token_t) { .type = id, .value = _id });
					} else {
						printf("lexer_tokenize: unknown token %c\n", current);
						abort();
					}
				}
				break;
		}
	} while ((current = input[++idx]) != '\0');
}

void lexer_print(dynamic_array_t* token) {
	dynamic_array_iterator_t iterator;
	dynamic_array_iterator_init(token, &iterator);

	while (dynamic_array_iterator_has_next(&iterator)) {
		lexer_token_t* token = (lexer_token_t*) dynamic_array_iterator_next(&iterator);

		switch (token->type) {
			case number:
				printf("type: number: %d\n", token->value);
				break;
			
			case plus:
				printf("type: plus\n");
				break;
			
			case minus:
				printf("type: minus\n");
				break;

			case multiply:
				printf("type: multiply\n");
				break;

			case divide:
				printf("type: divide\n");
				break;

			case modulo:
				printf("type: modulo\n");
				break;
			
			case lparen:
				printf("type: lparen\n");
				break;
			
			case rparen:
				printf("type: rparen\n");
				break;

			case comma:
				printf("type: comma\n");
				break;
			
			case id:
				printf("type: id: %s\n", token->value);
				break;

			default:
				printf("type: unknown (%d)\n", token->type);
				break;
		}
	}
}