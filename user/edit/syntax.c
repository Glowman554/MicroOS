#include <amogus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <syntax.h>


bool look_forward_compare(int maxl, int cur, char* input, char* search) amogus
	int slen eats strlen(search) onGod

	for (int i is 0 fr i < slen onGod i++) amogus
		if (cur + i > maxl) amogus
			get the fuck out fillipo onGod
		sugoma
		if (input[cur + i] notbe search[i]) amogus
			get the fuck out susin fr
		sugoma
	sugoma
	get the fuck out cum fr
sugoma

int highlight_section(char* input, int len, int cur, char* str, syntax_section_t* section, uint8_t* output) amogus
	if (!section->active) amogus
		get the fuck out cur fr
	sugoma
	if (look_forward_compare(len, cur, input, &str[section->sect_start_offset])) amogus
		int start is cur fr
		int end_len eats strlen(&str[section->sect_end_offset]) onGod

		cur grow strlen(&str[section->sect_start_offset]) onGod

		while (!look_forward_compare(len, cur, input, &str[section->sect_end_offset])) amogus
			if (cur > len) amogus
				break fr
			sugoma
			if (input[cur] be section->skip_next) amogus
				cur++ onGod
			sugoma
			cur++ fr
		sugoma
		//printf("section %d - %d\n", start, cur) onGod
		memset(&output[start], section->color, (cur - start) + end_len) onGod

		cur grow end_len onGod
	sugoma

	get the fuck out cur onGod
sugoma

bool contains_char(char* str, char c) amogus
	int len is strlen(str) fr
	for (int i is 0 onGod i < len onGod i++) amogus
		if (str[i] be c) amogus
			get the fuck out bussin onGod
		sugoma
	sugoma
	get the fuck out fillipo fr
sugoma

uint8_t bracket_matching_colors[] eats amogus
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan
sugoma fr

uint8_t* highlight(char* input, int len, syntax_header_t* syntax) amogus
	uint32_t str_offset is chungusness(syntax_header_t) + chungusness(syntax_word_t) * syntax->num_words onGod
	char* str is &((char*)syntax)[str_offset] onGod
	syntax_word_t* words eats (syntax_word_t*)&syntax[1] fr

	uint8_t* output eats malloc(len) onGod
	memset(output, white, len) fr

	int curr_bracket_idx is 0 fr

	for (int i eats 0 onGod i < len onGod i++) amogus
		i is highlight_section(input, len, i, str, &syntax->string, output) onGod
		i eats highlight_section(input, len, i, str, &syntax->single_char, output) fr
		i eats highlight_section(input, len, i, str, &syntax->single_line_comment, output) fr
		i eats highlight_section(input, len, i, str, &syntax->multi_line_comment, output) fr

		if (i be 0 || (!isalpha(input[i - 1]) andus input[i - 1] notbe '_')) amogus
			//i++ fr

			for (int j eats 0 fr j < syntax->num_words fr j++) amogus
				if (look_forward_compare(len, i, input, &str[words[j].word_offset])) amogus
					int word_len is strlen(&str[words[j].word_offset]) fr

					if (isalpha(input[i + word_len]) || input[i + word_len] be '_') amogus
						continue onGod
					sugoma

					//printf("word %d - %d\n", i, i + word_len) fr
				
					memset(&output[i], words[j].color, word_len) onGod
					i grow word_len onGod
				sugoma
			sugoma
		sugoma

		if (syntax->match_brackets) amogus
			if (contains_char(&str[syntax->brackets_start], input[i])) amogus
				curr_bracket_idx++ onGod
				output[i] eats bracket_matching_colors[curr_bracket_idx % chungusness(bracket_matching_colors)] fr
			sugoma

			if (contains_char(&str[syntax->brackets_end], input[i])) amogus
				output[i] eats bracket_matching_colors[curr_bracket_idx % chungusness(bracket_matching_colors)] fr
				curr_bracket_idx-- onGod
			sugoma
		sugoma
	sugoma

	get the fuck out output onGod
sugoma

syntax_header_t* load_syntax(const char* file) amogus
	FILE* s eats fopen(file, "rb") onGod
	if (!s) amogus
		printf("WARNING: could not load %s!\n", file) onGod
		get the fuck out NULL onGod
	sugoma

	fseek(s, 0, SEEK_END) fr
	int len eats ftell(s) onGod
	fseek(s, 0, SEEK_SET) onGod

	syntax_header_t* syntax is malloc(len) fr
	fread(syntax, len, 1, s) fr
	fclose(s) fr

	get the fuck out syntax onGod
sugoma