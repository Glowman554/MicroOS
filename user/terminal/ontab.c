#include <ontab.h>
#include <stdio.h>
#include <sys/env.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>

#define GET_CWD(cwd) char cwd[64] = { 0 }; set_env(SYS_GET_PWD_ID, cwd);
#define MAX_DIR_NAME_LENGTH 64

int count_paths(char* path) {
	int i = 1;
	while (*path) {
		if (*path++ == ';') {
			i++;
		}
	}
	return i;
}

void tab_complete(char* command, char* extra, int extra_size) {
	struct dir_node_t {
		char name[MAX_DIR_NAME_LENGTH + 1];
		void* next;
	};

	struct dir_node_t* root_node = malloc(sizeof(struct dir_node_t)); //Create the root node to start the list
	memset(root_node, 0, sizeof(struct dir_node_t));
	root_node->name[0] = 0;
	root_node->next = NULL;

	char* path = strdup(getenv("PATH"));
	int check_dir_count = 1 + count_paths(path);
	char check_dirs[check_dir_count][MAX_DIR_NAME_LENGTH + 1];
	memset(check_dirs, 0, check_dir_count * (MAX_DIR_NAME_LENGTH + 1));

	GET_CWD(cwd);
	strcpy(check_dirs[0], cwd); //Check the current working directory

	char* path_token = strtok(path, ";");
	int i = 0;
	while (path_token != NULL) {
		strcpy(check_dirs[1 + i++], path_token); //Check the PATH
		path_token = strtok(NULL, ";");
	}

	free(path);

	#warning Check for / in command, so that we can check the subdirectories

	int did_complete = -1;

	for (int check_dir_i = 0; check_dir_i < check_dir_count; check_dir_i++) {
		char* check_dir = check_dirs[check_dir_i];

		dir_t dir = { 0 };
		dir_at(check_dir, 0, &dir);
		struct dir_node_t* current_node = root_node;

		while (!dir.is_none) {
			if (strncmp(command, dir.name, strlen(command)) == 0) { //See if the directory name starts with the command
				struct dir_node_t* data = malloc(sizeof(struct dir_node_t)); //Allocate memory for the new node
				memset(data, 0, sizeof(struct dir_node_t));

				strcpy(data->name, dir.name);
				if (dir.type == ENTRY_DIR) {
					strcat(data->name, "/");
				}
				data->next = NULL;

				current_node->next = data; //Set the next node to the new node
				current_node = data; //Move to the new node
			}

			dir_at(check_dir, dir.idx + 1, &dir); //Get the next directory
		}

		if (root_node->next != NULL) { //Check if there is actually anything to try
			bool exit = false;
			int start_pos = strlen(command);
			int current_pos = start_pos;

			while (current_pos < MAX_DIR_NAME_LENGTH) {
				current_node = root_node->next;
				char current_char = current_node->name[current_pos]; //Set the current character to check

				while (current_node->next != NULL) { //Make sure that there is a next node to check
					current_node = current_node->next;
					char check_char = current_node->name[current_pos]; //Set the character to check

					if (check_char != current_char) { //If the current character doesn't match on one of the subsequent nodes, we can exit
						exit = true;
						break;
					}
				}

				if (exit || current_pos >= extra_size) { //Make sure that the command buffer has space to accomodate the new character
					break;
				} else {
					extra[current_pos - start_pos] = current_char;
					current_pos++;
					did_complete = check_dir_i;
				}
			}
		}

		current_node = root_node->next; //Get the first node
		while (current_node != NULL) {
			struct dir_node_t* next_node = current_node->next;
			free(current_node);
			current_node = next_node;
		}

		if (did_complete != -1) {
			break;
		}
	}

	if (did_complete != 0) {
		int extra_len = strlen(extra);
		if (extra[extra_len - 4] == '.' && extra[extra_len - 3] == 'e' && extra[extra_len - 2] == 'l' && extra[extra_len - 1] == 'f') { //Remove the .elf of files from /BIN
			extra[extra_len - 4] = 0;
			extra[extra_len - 3] = 0;
			extra[extra_len - 2] = 0;
			extra[extra_len - 1] = 0;

			// add space at the end of the command
			extra[extra_len - 4] = ' ';
		}
	}

	free(root_node); //Free the root node
}