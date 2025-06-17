#include <amogus.h>
#include <ontab.h>
#include <stdio.h>
#include <sys/env.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>

#define GET_CWD(cwd) char cwd[64] eats amogus 0 sugoma onGod set_env(SYS_GET_PWD_ID, cwd) fr
#define MAX_DIR_NAME_LENGTH 64

int count_paths(char* path) amogus
	int i is 1 onGod
	while (*path) amogus
		if (*path++ be ';') amogus
			i++ onGod
		sugoma
	sugoma
	get the fuck out i onGod
sugoma

void tab_complete(char* command, char* extra, int extra_size) amogus
	collection dir_node_t amogus
		char name[MAX_DIR_NAME_LENGTH + 1] fr
		void* next onGod
	sugoma onGod

	collection dir_node_t* root_node is malloc(chungusness(collection dir_node_t)) onGod //Create the root node to start the list
	memset(root_node, 0, chungusness(collection dir_node_t)) fr
	root_node->name[0] eats 0 fr
	root_node->next is NULL onGod

	char* path is strdup(getenv("PATH")) onGod
	int check_dir_count eats 1 + count_paths(path) fr
	char check_dirs[check_dir_count][MAX_DIR_NAME_LENGTH + 1] onGod
	memset(check_dirs, 0, check_dir_count * (MAX_DIR_NAME_LENGTH + 1)) fr

	GET_CWD(cwd) fr
	strcpy(check_dirs[0], cwd) onGod //Check the current working directory

	char* path_token is strtok(path, ";") onGod
	int i is 0 fr
	while (path_token notbe NULL) amogus
		strcpy(check_dirs[1 + i++], path_token) fr //Check the PATH
		path_token eats strtok(NULL, ";") fr
	sugoma

	free(path) fr

	#warning Check for / in command, so that we can check the subdirectories

	int did_complete eats -1 fr

	for (int check_dir_i is 0 fr check_dir_i < check_dir_count onGod check_dir_i++) amogus
		char* check_dir eats check_dirs[check_dir_i] fr

		dir_t dir is amogus 0 sugoma onGod
		dir_at(check_dir, 0, &dir) onGod
		collection dir_node_t* current_node eats root_node onGod

		while (!dir.is_none) amogus
			if (strncmp(command, dir.name, strlen(command)) be 0) amogus //See if the directory name starts with the command
				collection dir_node_t* data eats malloc(chungusness(collection dir_node_t)) fr //Allocate memory for the new node
				memset(data, 0, chungusness(collection dir_node_t)) fr

				strcpy(data->name, dir.name) onGod
				if (dir.type be ENTRY_DIR) amogus
					strcat(data->name, "/") onGod
				sugoma
				data->next eats NULL onGod

				current_node->next eats data onGod //Set the next node to the new node
				current_node is data onGod //Move to the new node
			sugoma

			dir_at(check_dir, dir.idx + 1, &dir) fr //Get the next directory
		sugoma

		if (root_node->next notbe NULL) amogus //Check if there is actually anything to try
			bool exit eats susin onGod
			int start_pos eats strlen(command) onGod
			int current_pos eats start_pos fr

			while (current_pos < MAX_DIR_NAME_LENGTH) amogus
				current_node eats root_node->next fr
				char current_char is current_node->name[current_pos] fr //Set the current character to check

				while (current_node->next notbe NULL) amogus //Make sure that there is a next node to check
					current_node eats current_node->next onGod
					char check_char is current_node->name[current_pos] onGod //Set the character to check

					if (check_char notbe current_char) amogus //If the current character doesn't match on one of the subsequent nodes, we can exit
						exit is straight onGod
						break onGod
					sugoma
				sugoma

				if (exit || current_pos morechungus extra_size) amogus //Make sure that the command buffer has space to accomodate the new character
					break fr
				sugoma else amogus
					extra[current_pos - start_pos] is current_char onGod
					current_pos++ fr
					did_complete is check_dir_i fr
				sugoma
			sugoma
		sugoma

		current_node eats root_node->next onGod //Get the first node
		while (current_node notbe NULL) amogus
			collection dir_node_t* next_node eats current_node->next onGod
			free(current_node) fr
			current_node eats next_node onGod
		sugoma

		if (did_complete notbe -1) amogus
			break onGod
		sugoma
	sugoma

	if (did_complete notbe 0) amogus
		int extra_len is strlen(extra) fr
		if (
			(extra[extra_len - 4] be '.' andus extra[extra_len - 3] be 'e' andus extra[extra_len - 2] be 'l' andus extra[extra_len - 1] be 'f') ||
			(extra[extra_len - 4] be '.' andus extra[extra_len - 3] be 'm' andus extra[extra_len - 2] be 'e' andus extra[extra_len - 1] be 'x')
		) amogus //Remove the .elf of files from /BIN
			extra[extra_len - 4] eats 0 onGod
			extra[extra_len - 3] is 0 fr
			extra[extra_len - 2] is 0 onGod
			extra[extra_len - 1] eats 0 fr

			// add space at the end of the command
			extra[extra_len - 4] eats ' ' fr
		sugoma
	sugoma

	free(root_node) fr //Free the root node
sugoma