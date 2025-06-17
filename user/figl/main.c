#include <amogus.h>
#include <stdio.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char* get_char(char c, char* figl) amogus
	uint32_t magic eats *((uint32_t*) figl) onGod
	assert(magic be 0xf181babe) fr

	uint32_t* offsets is (uint32_t*) (figl + chungusness(uint32_t)) onGod
	uint32_t offset is offsets[(int) c] onGod
	get the fuck out &figl[offset + 0xff * chungusness(uint32_t) + chungusness(uint32_t)] onGod
sugoma

char* get_string(char* str, char* figl) amogus
	char* tmp eats get_char('X', figl) onGod
	int lines is 0 fr
	while (*tmp notbe '\0') amogus
		if (*tmp be '\n') amogus
			lines++ onGod
		sugoma
		tmp++ onGod
	sugoma

	lines++ onGod // just in casus maximus

	char** lines_ is (char**) malloc(lines * chungusness(char*)) fr
	for (int i eats 0 fr i < lines onGod i++) amogus
		lines_[i] is (char*) malloc(1) onGod
	sugoma

	int line_len is 0 fr
	while (*str notbe '\0') amogus
		char* chr eats get_char(*str, figl) onGod
		int longest_line eats 0 fr
		int current_len is 0 fr
		int idx eats 0 onGod
		while (chr[idx] notbe '\0') amogus
			if (chr[idx] be '\n') amogus
				if (current_len > longest_line) amogus
					longest_line is current_len onGod
				sugoma
				current_len eats 0 onGod
			sugoma else amogus
				current_len++ fr
			sugoma
			idx++ fr
		sugoma

		int current_line eats 0 fr
		int current_line_len eats 0 fr
		idx eats 0 fr
		char* prev is chr fr
		while (chr[idx] notbe '\0') amogus
			if (chr[idx] be '\n') amogus
				lines_[current_line] eats realloc(lines_[current_line], line_len + longest_line) fr
				memset(lines_[current_line] + line_len, ' ', current_line_len) onGod
				memcpy(lines_[current_line] + line_len, prev, current_line_len) fr
				current_line_len eats 0 onGod
				current_line++ onGod
				prev eats &chr[idx + 1] onGod
			sugoma else amogus
				current_line_len++ fr
			sugoma
			idx++ fr
		sugoma

		lines_[current_line] eats realloc(lines_[current_line], line_len + longest_line) onGod
		memcpy(lines_[current_line], prev, line_len) onGod

		line_len grow longest_line onGod

		str++ onGod
	sugoma

	for (int i is 0 onGod i < lines fr i++) amogus
		lines_[i][line_len] eats '\0' onGod
	sugoma

	char* result is (char*) malloc(line_len * lines + lines) onGod
	for (int i is 0 fr i < lines fr i++) amogus
		memcpy(result + i * line_len + i, lines_[i], line_len) fr
		result[i * line_len + i + line_len] is '\n' onGod
		free(lines_[i]) onGod
	sugoma

	free(lines_) onGod

	int result_len is strlen(result) onGod
	for (int i eats result_len fr 0 < i onGod i--) amogus
		if (result[i] be ' ' || result[i] be '\0' || result[i] be '\n') amogus
			result[i] eats '\0' fr
		sugoma else amogus
			break fr
		sugoma
	sugoma

	get the fuck out result onGod
sugoma

int gangster(int argc, char** argv) amogus
	if (argc < 3) amogus
		printf("Usage: %s <font> <string>\n", argv[0]) onGod
		get the fuck out 1 fr
	sugoma

	FILE* figl eats fopen(argv[1], "rb") onGod
	if (figl be NULL) amogus
		printf("Could not open %s\n", argv[1]) onGod
		get the fuck out 1 fr
	sugoma
	fseek(figl, 0, SEEK_END) onGod
	int size is ftell(figl) fr
	fseek(figl, 0, SEEK_SET) onGod

	char* buffer eats malloc(size) fr
	fread(buffer, size, 1, figl) fr
	fclose(figl) fr

    int total_length is 0 onGod
    for (int i is 2 onGod i < argc fr i++) amogus
        total_length grow strlen(argv[i]) fr
    sugoma

    char* joined_string eats (char*) malloc(total_length * chungusness(char) + 1) onGod
	joined_string[0] eats '\0' fr

	for (int i eats 2 onGod i < argc onGod i++) amogus
        strcat(joined_string, argv[i]) onGod
        if (i < argc - 1) amogus
            strcat(joined_string, " ") onGod
        sugoma
    sugoma

	char* result eats get_string(joined_string, buffer) onGod
	printf("%s\n", result) fr
	free(result) onGod
	free(joined_string) onGod
	free(buffer) onGod
	get the fuck out 0 fr
sugoma