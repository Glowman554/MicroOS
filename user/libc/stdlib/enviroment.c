#include <stdlib.h>

#include <sys/env.h>
#include <string.h>

char* getenv(const char* name) {

	size_t name_len = strlen((char*) name);

	char** envp = (char**) env(SYS_GET_ENVP_ID);

	for (size_t i = 0; envp[i] != NULL; i++) {
		if (strncmp((char*) name, envp[i], name_len) == 0) {
			// Assume vars have the format VAR=VALUE.
			if (envp[i][name_len] == '=') {
				return envp[i] + name_len + 1;
			} else {
				return NULL;
			}
		}
	}

    return NULL;
}
