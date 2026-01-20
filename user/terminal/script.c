#include <script.h>
#include <argv_tools.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <commands.h>

void run_script(char* path, char** argv, int argc) {
	char resolved_path[256] = { 0 };
	bool canresolve = resolve(path, resolved_path);
	if (!canresolve) {
		printf("Error: Could not resolve path: '%s'\n", path);
		exit(-1);
	}
	// printf("Running script: '%s'\n", path);

	for (int i = 0; i < argc; i++) {
		char export_command[512] = { 0 };
		sprintf(export_command, "export %d=%s", i, argv[i]);
		export(export_command, NULL);
	}

	FILE* file = fopen(resolved_path, "r");
	if (file == NULL) {
		printf("Error: Failed to open script file: '%s'\n", path);
		exit(-1);
	}

	fsize(file, file_size);

	char* script_data = malloc(file_size+ 1);
	memset(script_data, 0, file_size + 1);
	fread(script_data, file_size, 1, file);

	int line_number = 0;
	char current_line[256] = { 0 };
	int script_idx = 0;
	int idx = 0;


	do {
		idx = 0;
		while (true) {
			if (script_idx >= file_size) {
				break;
			}

			if (script_data[script_idx] == '\n') {
				current_line[idx] = '\0';
				script_idx++;
				line_number++;
				break;
			} else {
				current_line[idx] = script_data[script_idx];
			}

			script_idx++;
			idx++;
		}

		if (current_line[0] == '#') {
			memset(current_line, 0, sizeof(current_line));
			continue;
		}

		if (current_line[0] == '\0') {
			memset(current_line, 0, sizeof(current_line));
			continue;
		}

		//printf("In line: '%s'\n", current_line);
		char* processed_line = process_line(current_line, true);
		if (processed_line[0] == '\0') {
			continue;
		}
		//printf("Out line: '%s'\n", processed_line);

		char command[256] = { 0 };
		memcpy(command, processed_line, strlen(processed_line) + 1);

		bool should_break;
		bool command_found = command_received(command, &should_break, NULL); //This should block while command is running.
		if (!command_found) {
			printf("Error: Command not found: '%s', line %d\n", command, line_number);
			exit(-1);
		}
		memset(current_line, 0, sizeof(current_line));

		if (should_break == true) {
			break;
		}
	} while (script_idx < file_size);

	fclose(file); // we still use the file object in the loop so we can only close it here.
	free(script_data);
	return;
}