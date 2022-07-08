#include <argv_tools.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

int get_command_type(char* command, int* token_pos, bool* double_pipe_symbol) {
	*double_pipe_symbol = false;
	*token_pos = 0;
	
	int command_length = strlen(command);

	bool quote_open = false;
	bool double_quote_open = false;
	bool special_char_next = false;

	for (int i = 0; i < command_length; i++) {
		if (command[i] == '|' && !(quote_open || double_quote_open || special_char_next)) {
			*token_pos = i;
			return PIPE_PROC;
		} else if (command[i] == '>' && !(quote_open || double_quote_open || special_char_next)) {
			*token_pos = i;
			if (i + 1 < command_length && command[i + 1] == '>') {
				*token_pos = i + 1;
				*double_pipe_symbol = true;
				return PIPE_FILE_APPEND;
			} else {
				return PIPE_FILE;
			}
		} else if (command[i] == '&' && !(quote_open || double_quote_open || special_char_next)) {
			*token_pos = i;
			return AND_RUN;
		} else if (command[i] == '\\') {
			special_char_next = true;
		} else if (command[i] == '\"') {
			if (special_char_next || double_quote_open) {
				special_char_next = false;
			} else {
				quote_open = !quote_open;
			}
		} else if (command[i] == '\'') {
			if (special_char_next || quote_open) {
				special_char_next = false;
			} else {
				double_quote_open = !double_quote_open;
			}
		} else {
			if (special_char_next) {
				special_char_next = false;
			}
		}
	}

	return NORMAL;
}

char* read_env(char* in) {
	char tmp[512] = { 0 };
	int tmp_idx = 0;

	for (int i = 0; i < strlen(in); i++) {
		if (in[i] == '$') {
			char env_var[256] = { 0 };
			int env_var_idx = 0;

			i++;

			while (((in[i] >= 'a' && in[i] <= 'z') || (in[i] >= 'A' && in[i] <= 'Z') || (in[i] >= '0' && in[i] <= '9') || (in[i] == '_' || in[i] == '?')) && in[i] != 0) {
				env_var[env_var_idx] = in[i];
				env_var_idx++;
				i++;
			}

			// printf("env_var: %s\n", env_var);
			char* env_var_value = getenv(env_var);
			// printf("env_var_value: %s\n", env_var_value);

			if (env_var_value != NULL) {
				strcat(tmp, env_var_value);
				tmp_idx += strlen(env_var_value);
			}

			i--;
		} else {
			tmp[tmp_idx] = in[i];
			tmp_idx++;
		}
	}

	char* out = malloc(strlen(tmp) + 1);
	memset(out, 0, strlen(tmp) + 1);
	memcpy(out, tmp, strlen(tmp) + 1);
	return out;
}

char** argv_env_process(char** in) {
	int curr_arg = 0;
	while (in[curr_arg] != NULL) {
		in[curr_arg] = read_env(in[curr_arg]);
		curr_arg++;
	}

	return in;
}

char** argv_split(char* str) {
	int len = strlen(str);

	int argc = 1;
	bool quote_open = false;
	bool double_quote_open = false;
	bool special_char_next = false;

	for (int i = 0; i < len; i++) {
		if(str[i] == ' ') {
			if (!quote_open && !double_quote_open) {
				argc++;
			}
		} else if (str[i] == '\\') {
			special_char_next = true;
		} else if (str[i] == '\"') {
			if (special_char_next || double_quote_open) {
				special_char_next = false;
			} else {
				quote_open = !quote_open;
			}
		} else if (str[i] == '\'') {
			if (special_char_next || quote_open) {
				special_char_next = false;
			} else {
				double_quote_open = !double_quote_open;
			}
		}
	}

	char** argv = malloc(sizeof(char*) * (argc + 1));

	argc = 1;
	argv[0] = &str[0];

	quote_open = false;
	double_quote_open = false;
	special_char_next = false;

	for (int i = 0; i < len; i++) {
		if(str[i] == ' ') {
			if (!quote_open && !double_quote_open) {
				argv[argc] = &str[i + 1];
				str[i] = 0;
				argc++;
			}
		} else if (str[i] == '\\') {
			special_char_next = true;
		} else if (str[i] == '\"') {
			if (special_char_next || double_quote_open) {
				special_char_next = false;
			} else {
				quote_open = !quote_open;
			}
		} else if (str[i] == '\'') {
			if (special_char_next || quote_open) {
				special_char_next = false;
			} else {
				double_quote_open = !double_quote_open;
			}
		}
	}

	argv[argc] = NULL;

	return argv;
}

void free_argv(char** argv) {
	for (int i = 0; argv[i] != NULL; i++) {
		free(argv[i]);
	}
	free(argv);
}

bool is_quote_open(char* command) {
	bool quote_open = false;
	bool double_quote_open = false;
	bool special_char_next = false;

	int len = strlen(command);

	for (int i = 0; i < len; i++) {
		if (command[i] == '\\') {
			special_char_next = true;
		} else if (command[i] == '\"') {
			if (special_char_next || double_quote_open) {
				special_char_next = false;
			} else {
				quote_open = !quote_open;
			}
		} else if (command[i] == '\'') {
			if (special_char_next || quote_open) {
				special_char_next = false;
			} else {
				double_quote_open = !double_quote_open;
			}
		}
	}

	return double_quote_open || quote_open;
}

char* process_line(char* command, bool check_hashtag) {
	int i, j;
	int command_length = strlen(command);

	char* output_1 = command;
	bool text_found = false;

	for (i = 0, j = 0; i < command_length; i++, j++) {
		if (command[i] == ' ' && !text_found) {
			j--;
		} else if (command[i] == '#' && check_hashtag) {
			output_1[j] = 0;
			break;
		} else {
			output_1[j] = command[i];
			text_found = true;
		}
	}
	output_1[j] = 0;

	command_length = strlen(output_1);
	if (command_length == 0) {
		return output_1;
	}

	char* output_2 = output_1;

	for (i = command_length - 1; i > 0; i--) {
		if (output_1[i] == ' ') {
			output_2[i] = 0;
		} else {
			break;
		}
	}

	return output_2;
}