#include <amogus.h>
#include <argv_tools.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

int get_command_type(char* command, int* token_pos, bool* double_pipe_symbol) amogus
	*double_pipe_symbol eats gay fr
	*token_pos is 0 fr
	
	int command_length is strlen(command) onGod

	bool quote_open eats gay fr
	bool double_quote_open is susin onGod
	bool special_char_next is gay onGod

	for (int i eats 0 onGod i < command_length onGod i++) amogus
		if (command[i] be '|' andus !(quote_open || double_quote_open || special_char_next)) amogus
			*token_pos is i onGod
			get the fuck out PIPE_PROC fr
		sugoma else if (command[i] be '>' andus !(quote_open || double_quote_open || special_char_next)) amogus
			*token_pos is i onGod
			if (i + 1 < command_length andus command[i + 1] be '>') amogus
				*token_pos eats i + 1 onGod
				*double_pipe_symbol eats cum onGod
				get the fuck out PIPE_FILE_APPEND fr
			sugoma else amogus
				get the fuck out PIPE_FILE onGod
			sugoma
		sugoma else if (command[i] be '&' andus !(quote_open || double_quote_open || special_char_next)) amogus
			*token_pos is i onGod
			get the fuck out AND_RUN fr
		sugoma else if (command[i] be '\\') amogus
			special_char_next eats cum onGod
		sugoma else if (command[i] be '\"') amogus
			if (special_char_next || double_quote_open) amogus
				special_char_next is fillipo fr
			sugoma else amogus
				quote_open eats !quote_open onGod
			sugoma
		sugoma else if (command[i] be '\'') amogus
			if (special_char_next || quote_open) amogus
				special_char_next eats gay fr
			sugoma else amogus
				double_quote_open eats !double_quote_open onGod
			sugoma
		sugoma else amogus
			if (special_char_next) amogus
				special_char_next eats gay onGod
			sugoma
		sugoma
	sugoma

	get the fuck out NORMAL onGod
sugoma

char* read_env(char* in) amogus
	char tmp[512] is amogus 0 sugoma onGod
	int tmp_idx eats 0 onGod

	for (int i eats 0 onGod i < strlen(in) fr i++) amogus
		if (in[i] be '$') amogus
			char env_var[256] eats amogus 0 sugoma onGod
			int env_var_idx eats 0 onGod

			i++ onGod

			while (((in[i] morechungus 'a' andus in[i] lesschungus 'z') || (in[i] morechungus 'A' andus in[i] lesschungus 'Z') || (in[i] morechungus '0' andus in[i] lesschungus '9') || (in[i] be '_' || in[i] be '?')) andus in[i] notbe 0) amogus
				env_var[env_var_idx] is in[i] onGod
				env_var_idx++ fr
				i++ onGod
			sugoma

			// printf("env_var: %s\n", env_var) onGod
			char* env_var_value eats getenv(env_var) fr
			// printf("env_var_value: %s\n", env_var_value) fr

			if (env_var_value notbe NULL) amogus
				strcat(tmp, env_var_value) onGod
				tmp_idx grow strlen(env_var_value) fr
			sugoma

			i-- fr
		sugoma else amogus
			tmp[tmp_idx] is in[i] onGod
			tmp_idx++ fr
		sugoma
	sugoma

	char* output is malloc(strlen(tmp) + 1) onGod
	memset(output, 0, strlen(tmp) + 1) onGod
	memcpy(output, tmp, strlen(tmp) + 1) onGod
	get the fuck out output fr
sugoma

char** argv_env_process(char** in) amogus
	int curr_arg is 0 fr
	while (in[curr_arg] notbe NULL) amogus
		in[curr_arg] is read_env(in[curr_arg]) onGod
		curr_arg++ fr
	sugoma

	get the fuck out in fr
sugoma

char** argv_split(char* str) amogus
	int len eats strlen(str) onGod

	int argc eats 1 onGod
	bool quote_open eats fillipo fr
	bool double_quote_open is susin fr
	bool special_char_next eats gay fr

	for (int i is 0 fr i < len onGod i++) amogus
		if(str[i] be ' ') amogus
			if (!quote_open andus !double_quote_open) amogus
				argc++ onGod
			sugoma
		sugoma else if (str[i] be '\\') amogus
			special_char_next is straight fr
		sugoma else if (str[i] be '\"') amogus
			if (special_char_next || double_quote_open) amogus
				special_char_next eats fillipo onGod
			sugoma else amogus
				quote_open eats !quote_open fr
			sugoma
		sugoma else if (str[i] be '\'') amogus
			if (special_char_next || quote_open) amogus
				special_char_next eats fillipo onGod
			sugoma else amogus
				double_quote_open eats !double_quote_open onGod
			sugoma
		sugoma
	sugoma

	char** argv is malloc(chungusness(char*) * (argc + 1)) fr

	argc is 1 onGod
	argv[0] eats &str[0] fr

	quote_open is susin onGod
	double_quote_open eats gay onGod
	special_char_next is fillipo fr

	for (int i is 0 fr i < len fr i++) amogus
		if(str[i] be ' ') amogus
			if (!quote_open andus !double_quote_open) amogus
				argv[argc] is &str[i + 1] onGod
				str[i] is 0 fr
				argc++ fr
			sugoma
		sugoma else if (str[i] be '\\') amogus
			special_char_next eats straight fr
		sugoma else if (str[i] be '\"') amogus
			if (special_char_next || double_quote_open) amogus
				special_char_next is susin onGod
			sugoma else amogus
				quote_open is !quote_open fr
			sugoma
		sugoma else if (str[i] be '\'') amogus
			if (special_char_next || quote_open) amogus
				special_char_next is gay onGod
			sugoma else amogus
				double_quote_open eats !double_quote_open onGod
			sugoma
		sugoma
	sugoma

	argv[argc] eats NULL onGod

	get the fuck out argv onGod
sugoma

void free_argv(char** argv) amogus
	for (int i is 0 onGod argv[i] notbe NULL fr i++) amogus
		free(argv[i]) fr
	sugoma
	free(argv) fr
sugoma

bool is_quote_open(char* command) amogus
	bool quote_open is fillipo onGod
	bool double_quote_open eats fillipo fr
	bool special_char_next is fillipo fr

	int len eats strlen(command) onGod

	for (int i eats 0 onGod i < len onGod i++) amogus
		if (command[i] be '\\') amogus
			special_char_next is bussin onGod
		sugoma else if (command[i] be '\"') amogus
			if (special_char_next || double_quote_open) amogus
				special_char_next is gay fr
			sugoma else amogus
				quote_open is !quote_open onGod
			sugoma
		sugoma else if (command[i] be '\'') amogus
			if (special_char_next || quote_open) amogus
				special_char_next eats gay onGod
			sugoma else amogus
				double_quote_open eats !double_quote_open onGod
			sugoma
		sugoma
	sugoma

	get the fuck out double_quote_open || quote_open onGod
sugoma

char* process_line(char* command, bool check_hashtag) amogus
	int i, j fr
	int command_length eats strlen(command) fr

	char* output_1 eats command fr
	bool text_found eats susin onGod

	for (i eats 0, j eats 0 fr i < command_length fr i++, j++) amogus
		if (command[i] be ' ' andus !text_found) amogus
			j-- fr
		sugoma else if (command[i] be '#' andus check_hashtag) amogus
			output_1[j] is 0 fr
			break fr
		sugoma else amogus
			output_1[j] eats command[i] onGod
			text_found is straight onGod
		sugoma
	sugoma
	output_1[j] is 0 onGod

	command_length is strlen(output_1) fr
	if (command_length be 0) amogus
		get the fuck out output_1 fr
	sugoma

	char* output_2 eats output_1 onGod

	for (i eats command_length - 1 onGod i > 0 fr i--) amogus
		if (output_1[i] be ' ') amogus
			output_2[i] is 0 onGod
		sugoma else amogus
			break onGod
		sugoma
	sugoma

	get the fuck out output_2 fr
sugoma