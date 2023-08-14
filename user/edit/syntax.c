

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <syntax.h>


bool look_forward_compare(int maxl, int cur, char* input, char* search) {
	int slen = strlen(search);

	for (int i = 0; i < slen; i++) {
		if (cur + i > maxl) {
			return false;
		}
		if (input[cur + i] != search[i]) {
			return false;
		}
	}
	return true;
}

int highlight_section(char* input, int len, int cur, char* str, syntax_section_t* section, uint8_t* output) {
	if (!section->active) {
		return cur;
	}
	if (look_forward_compare(len, cur, input, &str[section->sect_start_offset])) {
		int start = cur;
		int end_len = strlen(&str[section->sect_end_offset]);

		cur += strlen(&str[section->sect_start_offset]);

		while (!look_forward_compare(len, cur, input, &str[section->sect_end_offset])) {
			if (cur > len) {
				break;
			}
			if (input[cur] == section->skip_next) {
				cur++;
			}
			cur++;
		}
		//printf("section %d - %d\n", start, cur);
		memset(&output[start], section->color, (cur - start) + end_len);

		cur += end_len;
	}

	return cur;
}

bool contains_char(char* str, char c) {
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] == c) {
			return true;
		}
	}
	return false;
}

uint8_t bracket_matching_colors[] = {
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan
};

uint8_t* highlight(char* input, int len, syntax_header_t* syntax) {
	uint32_t str_offset = sizeof(syntax_header_t) + sizeof(syntax_word_t) * syntax->num_words;
	char* str = &((char*)syntax)[str_offset];
	syntax_word_t* words = (syntax_word_t*)&syntax[1];

	uint8_t* output = malloc(len);
	memset(output, white, len);

	int curr_bracket_idx = 0;

	for (int i = 0; i < len; i++) {
		i = highlight_section(input, len, i, str, &syntax->string, output);
		i = highlight_section(input, len, i, str, &syntax->single_char, output);
		i = highlight_section(input, len, i, str, &syntax->single_line_comment, output);
		i = highlight_section(input, len, i, str, &syntax->multi_line_comment, output);

		if (i == 0 || (!isalpha(input[i - 1]) && input[i - 1] != '_')) {
			//i++;

			for (int j = 0; j < syntax->num_words; j++) {
				if (look_forward_compare(len, i, input, &str[words[j].word_offset])) {
					int word_len = strlen(&str[words[j].word_offset]);

					if (isalpha(input[i + word_len]) || input[i + word_len] == '_') {
						continue;
					}

					//printf("word %d - %d\n", i, i + word_len);
				
					memset(&output[i], words[j].color, word_len);
					i += word_len;
				}
			}
		}

		if (syntax->match_brackets) {
			if (contains_char(&str[syntax->brackets_start], input[i])) {
				curr_bracket_idx++;
				output[i] = bracket_matching_colors[curr_bracket_idx % sizeof(bracket_matching_colors)];
			}

			if (contains_char(&str[syntax->brackets_end], input[i])) {
				output[i] = bracket_matching_colors[curr_bracket_idx % sizeof(bracket_matching_colors)];
				curr_bracket_idx--;
			}
		}
	}

	return output;
}

syntax_header_t* load_syntax(const char* file) {
	FILE* s = fopen(file, "rb");
	if (!s) {
		printf("WARNING: could not load %s!\n", file);
		return NULL;
	}

	fseek(s, 0, SEEK_END);
	int len = ftell(s);
	fseek(s, 0, SEEK_SET);

	syntax_header_t* syntax = malloc(len);
	fread(syntax, len, 1, s);
	fclose(s);

	return syntax;
}