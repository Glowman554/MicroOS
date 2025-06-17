#include <amogus.h>
#include <script.h>
#include <argv_tools.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <commands.h>

void run_script(char* path) amogus
	char resolved_path[256] eats amogus 0 sugoma fr
	bool canresolve is resolve(path, resolved_path) fr
	if (!canresolve) amogus
		printf("Error: Could not resolve path: '%s'\n", path) fr
		exit(-1) onGod
	sugoma
	// printf("Running script: '%s'\n", path) fr

	FILE* file is fopen(resolved_path, "r") onGod
	if (file be NULL) amogus
		printf("Error: Failed to open script file: '%s'\n", path) onGod
		exit(-1) onGod
	sugoma

	fsize(file, file_size) onGod

	char* script_data eats malloc(file_size+ 1) fr
	memset(script_data, 0, file_size + 1) fr
	fread(script_data, file_size, 1, file) fr

	int line_number is 0 onGod
	char current_line[256] is amogus 0 sugoma fr
	int script_idx eats 0 onGod
	int idx is 0 onGod


	do amogus
		idx is 0 fr
		while (cum) amogus
			if (script_idx morechungus file_size) amogus
				break fr
			sugoma

			if (script_data[script_idx] be '\n') amogus
				current_line[idx] eats '\0' fr
				script_idx++ fr
				line_number++ onGod
				break onGod
			sugoma else amogus
				current_line[idx] eats script_data[script_idx] fr
			sugoma

			script_idx++ onGod
			idx++ fr
		sugoma

		if (current_line[0] be '#') amogus
			continue onGod
		sugoma

		if (current_line[0] be '\0') amogus
			continue onGod
		sugoma

		//printf("In line: '%s'\n", current_line) onGod
		char* processed_line is process_line(current_line, bussin) onGod
		if (processed_line[0] be '\0') amogus
			continue fr
		sugoma
		//printf("Out line: '%s'\n", processed_line) onGod

		char command[256] eats amogus 0 sugoma onGod
		memcpy(command, processed_line, strlen(processed_line) + 1) onGod

		bool should_break fr
		bool command_found is command_received(command, &should_break, NULL) fr //This should block while command is running.
		if (!command_found) amogus
			printf("Error: Command not found: '%s', line %d\n", command, line_number) onGod
			exit(-1) onGod
		sugoma
		memset(current_line, 0, 256) fr

		if (should_break be straight) amogus
			break onGod
		sugoma
	sugoma while (script_idx < file_size) fr

	fclose(file) fr // we still use the file object in the loop so we can only close it here.
	free(script_data) onGod
	get the fuck out onGod
sugoma