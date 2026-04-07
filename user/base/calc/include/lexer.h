#pragma once

#include <dynamic_array.h>

enum lexer_token_e {
	number,
	plus,
	minus,
	multiply,
	divide,
	modulo,
	lparen,
	rparen,
	comma,
	id
};


typedef struct {
	int type;
	void* value;
} lexer_token_t;

void lexer_delete(dynamic_array_t* token);
void lexer_tokenize(dynamic_array_t* token, char* input);
void lexer_print(dynamic_array_t* token);