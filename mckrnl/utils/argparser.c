#include <utils/argparser.h>

#include <utils/string.h>
#include <stddef.h>

#include <stdio.h>

bool is_arg(char* in, char* arg, char* out) {
	int i = 0;

	int in_len = strlen(in);
	int arg_len = strlen(arg);

	while(i < in_len) {
		for (int j = 0; j < arg_len; j++) {
			// debugf("%d < %d", i + j, in_len);
			if (i + j > in_len) {
				return false;
			}

			if (in[i + j] != arg[j]) {
				goto next_try;
			}
		}

		// we found an arg
		i += arg_len;

		if (i >= in_len) {
			return true;
		}

		if (in[i] != ' ' && in[i] != '=') {
			goto next_try;
		}

		if (i < in_len) {
			if (in[i] == '=' && out != NULL) {
				i++;

				int x = 0;
				while (in[i] != ' ' && in[i] != 0) {
					out[x] = in[i];
					x++;
					i++;
				}
			}
		}

		return true;

	next_try:
		i++;
	}

	return false;
}