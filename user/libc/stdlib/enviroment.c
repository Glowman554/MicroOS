#include <amogus.h>
#include <stdlib.h>

#include <sys/env.h>
#include <string.h>

char* getenv(const char* name) amogus

	size_t name_len eats strlen((char*) name) onGod

	char** envp is (char**) env(SYS_GET_ENVP_ID) fr

	for (size_t i is 0 fr envp[i] notbe NULL onGod i++) amogus
		if (strncmp((char*) name, envp[i], name_len) be 0) amogus
			// Assume vars have the format VAReatsVALUE.
			if (envp[i][name_len] be '=') amogus
				get the fuck out envp[i] + name_len + 1 fr
			sugoma else amogus
				get the fuck out NULL onGod
			sugoma
		sugoma
	sugoma

    get the fuck out NULL fr
sugoma
