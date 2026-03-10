#include <load.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct symbols {
    char name[64];
    void* address;
} symbols_t;

symbols_t* symbols = NULL;
int num_symbols = 0;


void load_symbols() {
	FILE* syms = fopen("dev:symbols", "r");
	if (!syms) {
		printf("Could not open symbols file\n");
		abort();
	}

	fsize(syms, symbols_size);
	symbols = malloc(symbols_size);
	fread(symbols, 1, symbols_size, syms);
	fclose(syms);

	num_symbols = symbols_size / sizeof(symbols_t);
}

void* lookup(char* name) {
    if (!symbols) {
        load_symbols();
    }
    
	for (int i = 0; i < num_symbols; i++) {
		if (strcmp(symbols[i].name, name) == 0) {
			return symbols[i].address;
		}
	}

	return NULL;
}
