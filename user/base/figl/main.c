#include <stdio.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char* get_char(char c, char* figl) {
	uint32_t magic = *((uint32_t*) figl);
	assert(magic == 0xf181babe);

	uint32_t* offsets = (uint32_t*) (figl + sizeof(uint32_t));
	uint32_t offset = offsets[(int) c];
	return &figl[offset + 0xff * sizeof(uint32_t) + sizeof(uint32_t)];
}

char* get_string(char* str, char* figl) {
	char* tmp = get_char('X', figl);
	int lines = 0;
	while (*tmp != '\0') {
		if (*tmp == '\n') {
			lines++;
		}
		tmp++;
	}

	lines++; // just in case

	char** lines_ = (char**) malloc(lines * sizeof(char*));
	for (int i = 0; i < lines; i++) {
		lines_[i] = (char*) malloc(1);
	}

	int line_len = 0;
	while (*str != '\0') {
		char* chr = get_char(*str, figl);
		int longest_line = 0;
		int current_len = 0;
		int idx = 0;
		while (chr[idx] != '\0') {
			if (chr[idx] == '\n') {
				if (current_len > longest_line) {
					longest_line = current_len;
				}
				current_len = 0;
			} else {
				current_len++;
			}
			idx++;
		}

		int current_line = 0;
		int current_line_len = 0;
		idx = 0;
		char* prev = chr;
		while (chr[idx] != '\0') {
			if (chr[idx] == '\n') {
				lines_[current_line] = realloc(lines_[current_line], line_len + longest_line);
				memset(lines_[current_line] + line_len, ' ', current_line_len);
				memcpy(lines_[current_line] + line_len, prev, current_line_len);
				current_line_len = 0;
				current_line++;
				prev = &chr[idx + 1];
			} else {
				current_line_len++;
			}
			idx++;
		}

		lines_[current_line] = realloc(lines_[current_line], line_len + longest_line);
		memcpy(lines_[current_line], prev, line_len);

		line_len += longest_line;

		str++;
	}

	for (int i = 0; i < lines; i++) {
		lines_[i][line_len] = '\0';
	}

	char* result = (char*) malloc(line_len * lines + lines);
	for (int i = 0; i < lines; i++) {
		memcpy(result + i * line_len + i, lines_[i], line_len);
		result[i * line_len + i + line_len] = '\n';
		free(lines_[i]);
	}

	free(lines_);

	int result_len = strlen(result);
	for (int i = result_len; 0 < i; i--) {
		if (result[i] == ' ' || result[i] == '\0' || result[i] == '\n') {
			result[i] = '\0';
		} else {
			break;
		}
	}

	return result;
}

int main(int argc, char** argv) {
	if (argc < 3) {
		printf("Usage: %s <font> <string>\n", argv[0]);
		return 1;
	}

	FILE* figl = fopen(argv[1], "rb");
	if (figl == NULL) {
		printf("Could not open %s\n", argv[1]);
		return 1;
	}
	fseek(figl, 0, SEEK_END);
	int size = ftell(figl);
	fseek(figl, 0, SEEK_SET);

	char* buffer = malloc(size);
	fread(buffer, size, 1, figl);
	fclose(figl);

    int total_length = 0;
    for (int i = 2; i < argc; i++) {
        total_length += strlen(argv[i]);
    }

    char* joined_string = (char*) malloc(total_length * sizeof(char) + 1);
	joined_string[0] = '\0';

	for (int i = 2; i < argc; i++) {
        strcat(joined_string, argv[i]);
        if (i < argc - 1) {
            strcat(joined_string, " ");
        }
    }

	char* result = get_string(joined_string, buffer);
	printf("%s\n", result);
	free(result);
	free(joined_string);
	free(buffer);
	return 0;
}