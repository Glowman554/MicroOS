#include <linker.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char* progname) {
	printf("Usage: %s [options] <input.o ...>\n", progname);
	printf("Options:\n");
	printf("  -o <file>        Output file (default: a.out)\n");
	printf("  -L <path>        Add library search path\n");
	printf("  -l <lib>         Link with library (searches for lib<lib>.a)\n");
	printf("  --load-addr <a>  Set load address (default: 0xB0000000)\n");
	printf("  -e <symbol>      Set entry point symbol (default: _start)\n");
	printf("  -h, --help       Show this help\n");
}

int main(int argc, char* argv[]) {
	linker_state_t* state = (linker_state_t*) malloc(sizeof(linker_state_t));
	if (!state) {
		fprintf(stderr, "linker: out of memory\n");
		return 1;
	}
	linker_init(state);

	const char* input_files[MAX_INPUT_FILES];
	int num_input_files = 0;

	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0) {
			if (i + 1 >= argc) {
				fprintf(stderr, "linker: -o requires an argument\n");
				free(state);
				return 1;
			}
			state->output_file = argv[++i];
		} else if (strcmp(argv[i], "-L") == 0) {
			if (i + 1 >= argc) {
				fprintf(stderr, "linker: -L requires an argument\n");
				free(state);
				return 1;
			}
			if (state->num_lib_paths >= MAX_LIB_PATHS) {
				fprintf(stderr, "linker: too many library paths\n");
				free(state);
				return 1;
			}
			state->lib_paths[state->num_lib_paths++] = argv[++i];
		} else if (strcmp(argv[i], "-l") == 0) {
			if (i + 1 >= argc) {
				fprintf(stderr, "linker: -l requires an argument\n");
				free(state);
				return 1;
			}
			if (state->num_libs >= MAX_LIBS) {
				fprintf(stderr, "linker: too many libraries\n");
				free(state);
				return 1;
			}
			state->libs[state->num_libs++] = argv[++i];
		} else if (strcmp(argv[i], "--load-addr") == 0) {
			if (i + 1 >= argc) {
				fprintf(stderr, "linker: --load-addr requires an argument\n");
				free(state);
				return 1;
			}
			i++;
			state->load_addr = (uint32_t) strtol(argv[i], NULL, 0);
		} else if (strcmp(argv[i], "-e") == 0) {
			if (i + 1 >= argc) {
				fprintf(stderr, "linker: -e requires an argument\n");
				free(state);
				return 1;
			}
			state->entry_name = argv[++i];
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			print_usage(argv[0]);
			free(state);
			return 0;
		} else if (argv[i][0] == '-') {
			fprintf(stderr, "linker: unknown option '%s'\n", argv[i]);
			free(state);
			return 1;
		} else {
			if (num_input_files >= MAX_INPUT_FILES) {
				fprintf(stderr, "linker: too many input files\n");
				free(state);
				return 1;
			}
			input_files[num_input_files++] = argv[i];
		}
	}

	if (num_input_files == 0) {
		fprintf(stderr, "linker: no input files\n");
		print_usage(argv[0]);
		free(state);
		return 1;
	}

	for (i = 0; i < num_input_files; i++) {
		if (linker_add_object(state, input_files[i]) < 0) {
			free(state);
			return 1;
		}
	}

	if (linker_resolve_libraries(state) < 0) {
		free(state);
		return 1;
	}

	if (linker_layout(state) < 0) {
		free(state);
		return 1;
	}

	if (linker_relocate(state) < 0) {
		free(state);
		return 1;
	}

	if (linker_write_elf(state, state->output_file) < 0) {
		free(state);
		return 1;
	}

	free(state);
	return 0;
}
