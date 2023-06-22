#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <keyboard_layout.h>
#include <buildin/number_parser.h>
#include <sys/file.h>

keymap_t from_file(char* path) {
	FILE* f = fopen(path, "r");
	if (f == NULL) {
		printf("Error: Could not open file %s (you are working on a new empty keymap now)\n", path);
		keymap_t keymap;
		memset(&keymap, 0, sizeof(keymap));
		return keymap;
	}

	keymap_t keymap;
	if (fread(&keymap, sizeof(keymap_t), 1, f) != sizeof(keymap_t)) {
		printf("Error: Could not read keymap from file %s\n", path);
		abort();
	}

	fclose(f);

	return keymap;
}

void save_file(char* path, keymap_t keymap) {
	char resolved_path[512] = { 0 };
	resolve(path, resolved_path);
	touch(resolved_path);
	FILE* f = fopen(path, "w");
	if (f == NULL) {
		printf("Error: Could not open file %s\n", path);
		abort();
	}

	if (fwrite(&keymap, sizeof(keymap_t), 1, f) != sizeof(keymap_t)) {
		printf("Error: Could not write keymap to file %s\n", path);
		abort();
	}

	fclose(f);
}

int main(int argc, char* argv[]) {
	bool normal_mode = false;
	bool alt_mode = false;
	bool shift_mode = false;

	char* file = NULL;

	int idx = 1;
	while (idx < argc) {
		if (strcmp(argv[idx], "-n") == 0) {
			normal_mode = true;
		} else if (strcmp(argv[idx], "-s") == 0) {
			shift_mode = true;
		} else if (strcmp(argv[idx], "-a") == 0) {
			alt_mode = true;
		} else if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s [-n] [-s] [-a] <file>\n", argv[0]);
			exit(0);
		} else {
			file = argv[idx];
		}

		idx++;
	}

	if (!normal_mode && !alt_mode && !shift_mode) {
		printf("Error: No mode specified\n");
		abort();
	}

	if (file == NULL) {
		printf("Error: No file specified\n");
		abort();
	}

	keymap_t keymap = from_file(file);

	while (true) {
		char input[512] = { 0 };
		printf("%s", "index of char to edit or exit or list > ");
		gets(input);

		if (strcmp(input, "exit") == 0) {
			break;
		}

		if (strcmp(input, "list") == 0) {
			char* layout;
			if (normal_mode) {
				layout = keymap.layout_normal;
			}
			if (shift_mode) {
				layout = keymap.layout_shift;
			}
			if (alt_mode) {
				layout = keymap.layout_alt;
			}

			for (int i = 0; i < 0xff; i++) {
				if (layout[i]) {
					printf("%d: %c (%d)\n", i, layout[i], layout[i]);
				}
			}
			continue;
		}

		int index;
		__libc_parse_number(input, &index);
		if (index < 0 || index >= 0xff) {
			printf("Error: Index out of bounds\n");
			continue;
		}

		printf("%s", "new char > ");
		gets(input);

		if (input[0] == '\\' && input[1] == 'd') {
			__libc_parse_number(input + 2, (int*) &input[0]);
		}

		printf("set %d to %c\n", index, input[0]);

		if (normal_mode) {
			keymap.layout_normal[index] = input[0];
		} else if (alt_mode) {
			keymap.layout_shift[index] = input[0];
		} else if (shift_mode) {
			keymap.layout_shift[index] = input[0];
		}
	}


	save_file(file, keymap);

	printf("Successfully wrote keymap to file %s\n", file);
}