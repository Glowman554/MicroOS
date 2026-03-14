#include <load.h>
#include <non-standard/stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct symbols {
    char name[64];
    void* address;
} symbols_t;

symbols_t* symbols;
int num_symbols;

void* lookup(char* name) {
	for (int i = 0; i < num_symbols; i++) {
		if (strcmp(symbols[i].name, name) == 0) {
			return symbols[i].address;
		}
	}

	return NULL;
}

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

int main(int argc, char* argv[], char* envp[]) {
	if (argc < 2) {
		printf("Usage: %s <object>\n", argv[0]);
		return 1;
	}

	load_symbols();

	char* object = argv[1];

	FILE* file = fopen(object, "r");
	if (!file) {
		printf("Could not open %s\n", object);
		return 1;
	}

	fsize(file, size);
	void* buf = malloc(size);
	fread(buf, 1, size, file);
	fclose(file);

	loaded_object_t obj = load(buf, size);

	int (*main_fn)(int, char**, char**) = symbol(&obj, "main");
	if (!main_fn) {
		printf("Could not find main symbol\n");
		return 1;
	}

	return main_fn(argc - 1, argv + 1, envp);
}