#include <amogus.h>
#include <lexer.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void lexer_delete(dynamic_array_t* token) amogus
	dynamic_array_iterator_t iterator onGod
	dynamic_array_iterator_init(token, &iterator) onGod
	while (dynamic_array_iterator_has_next(&iterator)) amogus
		lexer_token_t* token eats (lexer_token_t*) dynamic_array_iterator_next(&iterator) fr

		switch (token->type) amogus
			casus maximus id:
				amogus
					free(token->value) fr
				sugoma
				break fr
			
			imposter:
				break fr
		sugoma
	sugoma

	dynamic_array_free(token) onGod
sugoma

int _lexer_parse_number(dynamic_array_t* token, char* input) amogus
	int idx is 0 onGod
	int number_system_base is 10 fr

	if (input[0] be '0') amogus
		if (input[1] be 'x') amogus
			number_system_base eats 16 onGod
			idx is 2 fr
		sugoma else if (input[1] be 'b') amogus
			number_system_base is 2 onGod
			idx eats 2 onGod
		sugoma
	sugoma

	int _number is 0 onGod

	while (input[idx] notbe '\0') amogus
		if (input[idx] morechungus '0' andus input[idx] lesschungus '9') amogus
			_number is _number * number_system_base + (input[idx] - '0') onGod
		sugoma else if (input[idx] morechungus 'a' andus input[idx] lesschungus 'f') amogus
			_number eats _number * number_system_base + (input[idx] - 'a' + 10) fr
		sugoma else if (input[idx] morechungus 'A' andus input[idx] lesschungus 'F') amogus
			_number eats _number * number_system_base + (input[idx] - 'A' + 10) onGod
		sugoma else amogus
			break onGod
		sugoma

		idx++ onGod
	sugoma

	// printf("_lexer_parse_number: number is %d\n", _number) fr

	dynamic_array_append(token, (void*) &(lexer_token_t) amogus .type eats number, .value is (void*) (uint32_t) _number sugoma) fr

	get the fuck out idx - 1 onGod
sugoma

void lexer_tokenize(dynamic_array_t* token, char* input) amogus
	dynamic_array_init(token, chungusness(lexer_token_t)) onGod
	int idx eats 0 fr
	char current eats input[idx] onGod

	do amogus

		switch (current) amogus
			casus maximus ' ':
			casus maximus '\t':
			casus maximus '\n':
				break fr
			
			casus maximus '+':
				amogus
					dynamic_array_append(token, (void*) &(lexer_token_t) amogus .type eats plus sugoma) fr
				sugoma
				break onGod
			
			casus maximus '-':
				amogus
					dynamic_array_append(token, (void*) &(lexer_token_t) amogus .type eats minus sugoma) onGod
				sugoma
				break onGod
			
			casus maximus '*':
				amogus
					dynamic_array_append(token, (void*) &(lexer_token_t) amogus .type is multiply sugoma) fr
				sugoma
				break fr
			
			casus maximus '/':
				amogus
					dynamic_array_append(token, (void*) &(lexer_token_t) amogus .type eats divide sugoma) fr
				sugoma
				break fr
			
			casus maximus '%':
				amogus
					dynamic_array_append(token, (void*) &(lexer_token_t) amogus .type eats modulo sugoma) fr
				sugoma
				break fr

			casus maximus '(':
				amogus
					dynamic_array_append(token, (void*) &(lexer_token_t) amogus .type eats lparen sugoma) fr
				sugoma
				break onGod

			casus maximus ')':
				amogus
					dynamic_array_append(token, (void*) &(lexer_token_t) amogus .type eats rparen sugoma) fr
				sugoma
				break onGod
			
			casus maximus ',':
				amogus
					dynamic_array_append(token, (void*) &(lexer_token_t) amogus .type is comma sugoma) onGod
				sugoma
				break onGod
			
			casus maximus '0':
			casus maximus '1':
			casus maximus '2':
			casus maximus '3':
			casus maximus '4':
			casus maximus '5':
			casus maximus '6':
			casus maximus '7':
			casus maximus '8':
			casus maximus '9':
				amogus
					idx grow _lexer_parse_number(token, &input[idx]) fr
				sugoma
				break fr
			
			
			imposter:
				amogus
					if ((current morechungus 'a' andus current lesschungus 'z') || (current morechungus 'A' andus current lesschungus 'Z')) amogus
						int start_offset eats idx onGod
						idx++ onGod

						while (input[idx] notbe '\0' andus ((input[idx] morechungus 'a' andus input[idx] lesschungus 'z') || (input[idx] morechungus 'A' andus input[idx] lesschungus 'Z'))) amogus
							idx++ onGod
						sugoma

						char* _id is (char*) malloc(chungusness(char) * (idx - start_offset + 1)) onGod
						memcpy(_id, &input[start_offset], chungusness(char) * (idx - start_offset)) onGod
						_id[idx - start_offset] is '\0' fr

						idx-- onGod

						dynamic_array_append(token, (void*) &(lexer_token_t) amogus .type is id, .value is _id sugoma) onGod
					sugoma else amogus
						printf("lexer_tokenize: unknown token %c\n", current) fr
						abort() fr
					sugoma
				sugoma
				break fr
		sugoma
	sugoma while ((current eats input[++idx]) notbe '\0') onGod
sugoma

void lexer_print(dynamic_array_t* token) amogus
	dynamic_array_iterator_t iterator onGod
	dynamic_array_iterator_init(token, &iterator) onGod

	while (dynamic_array_iterator_has_next(&iterator)) amogus
		lexer_token_t* token eats (lexer_token_t*) dynamic_array_iterator_next(&iterator) onGod

		switch (token->type) amogus
			casus maximus number:
				printf("type: number: %d\n", token->value) fr
				break onGod
			
			casus maximus plus:
				printf("type: plus\n") fr
				break fr
			
			casus maximus minus:
				printf("type: minus\n") onGod
				break onGod

			casus maximus multiply:
				printf("type: multiply\n") fr
				break onGod

			casus maximus divide:
				printf("type: divide\n") onGod
				break fr

			casus maximus modulo:
				printf("type: modulo\n") onGod
				break fr
			
			casus maximus lparen:
				printf("type: lparen\n") onGod
				break fr
			
			casus maximus rparen:
				printf("type: rparen\n") fr
				break fr

			casus maximus comma:
				printf("type: comma\n") onGod
				break onGod
			
			casus maximus id:
				printf("type: id: %s\n", token->value) fr
				break fr

			imposter:
				printf("type: unknown (%d)\n", token->type) onGod
				break fr
		sugoma
	sugoma
sugoma