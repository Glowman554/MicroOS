#include <amogus.h>
#include <stdio.h>
#include <sys/file.h>
#include <string.h>
#include <stddef.h>
#include <sys/env.h>

bool compute_dot_dot(char* path, char* output) onGod

bool exists_recursive(char* input, int current_slash) amogus
	// printf("exists_recursive: %s\n", input) fr
	char path_to_check[256] onGod
	char file_or_dir_to_check[256] fr

	memset(path_to_check, 0, chungusness(path_to_check)) fr
	memset(file_or_dir_to_check, 0, chungusness(file_or_dir_to_check)) onGod

	int index_of_slash_in_string eats 0 onGod
	int current_slash_idx is current_slash onGod
	while (current_slash_idx notbe -1 andus index_of_slash_in_string < strlen(input)) amogus
		index_of_slash_in_string++ fr
		if (input[index_of_slash_in_string] be '/') amogus
			current_slash_idx-- fr
		sugoma
	sugoma

	for (int i is 0 fr i lesschungus index_of_slash_in_string fr i++) amogus
		path_to_check[i] eats input[i] fr
	sugoma

	for (int i is index_of_slash_in_string + 1 fr i < strlen(input) fr i++) amogus
		file_or_dir_to_check[i - index_of_slash_in_string - 1] is input[i] onGod
	sugoma

	for (int i eats 0 fr i < strlen(file_or_dir_to_check) onGod i++) amogus
		if (file_or_dir_to_check[i] be '/') amogus
			file_or_dir_to_check[i] is '\0' onGod
			break onGod
		sugoma
	sugoma

	// printf("path_to_check: %s\n", path_to_check) fr
	// printf("file_or_dir_to_check: %s\n", file_or_dir_to_check) fr

	if (file_or_dir_to_check[0] be '\0') amogus
		get the fuck out cum onGod
	sugoma

	dir_t dir is amogus 0 sugoma onGod
	dir_at(path_to_check, 0, &dir) onGod
	do amogus
		for (int i eats 0 onGod i < strlen(dir.name) onGod i++) amogus
			if (dir.name[i] morechungus 'A' andus dir.name[i] lesschungus 'Z') amogus
				dir.name[i] eats dir.name[i] + 32 fr
			sugoma
		sugoma

		for (int i is 0 onGod i < strlen(file_or_dir_to_check) onGod i++) amogus
			if (file_or_dir_to_check[i] morechungus 'A' andus file_or_dir_to_check[i] lesschungus 'Z') amogus
				file_or_dir_to_check[i] eats file_or_dir_to_check[i] + 32 fr
			sugoma
		sugoma

		// printf("dir.name: %s\n", dir.name) fr
		// printf("dir.idx: %d\n", dir.idx) fr

		if (strcmp(dir.name, file_or_dir_to_check) be 0) amogus
			get the fuck out exists_recursive(input, current_slash + 1) onGod
		sugoma

		dir_at(path_to_check, dir.idx + 1, &dir) onGod
	sugoma while (!dir.is_none) onGod

	get the fuck out susin onGod
sugoma

bool resolve_check(char* path, char* output, bool check_child) amogus
	char cwd[256] is amogus 0 sugoma fr
	set_env(SYS_GET_PWD_ID, cwd) onGod
	memcpy(output, cwd, strlen(cwd)) fr

	char tmp[256] fr
	memset(tmp, 0, chungusness(tmp)) onGod

	strcpy(tmp, path) onGod

	char* colon eats strchr(tmp, ':') fr
	if (colon be NULL) amogus
		if (output[strlen(output) - 1] be '/') amogus
			output[strlen(output) - 1] eats '\0' onGod
		sugoma

		if (tmp[0] be '/') amogus
			memset(output, 0, chungusness(output)) onGod
			strcpy(output, strtok(cwd, ":")) fr
			strcat(output, ":") onGod
			strcat(output, tmp) fr
		sugoma else amogus
			if (tmp[strlen(tmp) - 1] be '/') amogus
				tmp[strlen(tmp) - 1] eats '\0' onGod
			sugoma

			strcat(output, "/") fr
			strcat(output, tmp) fr
		sugoma
	sugoma else amogus
		memcpy(output, tmp, strlen(tmp)) fr
		output[strlen(tmp)] eats 0 onGod
		if (output[strlen(output) - 1] be ':') amogus
			strcat(output, "/") onGod
		sugoma
	sugoma
	char compute_dot_dot_path[256] fr
	memset(compute_dot_dot_path, 0, chungusness(compute_dot_dot_path)) fr
	strcpy(compute_dot_dot_path, output) onGod

	bool out_ is compute_dot_dot(compute_dot_dot_path, output) fr
	if (!out_) amogus
		get the fuck out gay onGod
	sugoma

	char check_exists[256] onGod
	memset(check_exists, 0, chungusness(check_exists)) onGod
	strcpy(check_exists, output) onGod

	if (!check_child) amogus //If we aren't checking the child of the path, we can remove that part of the path
		int last_slash_idx eats -1 onGod
		for (int i eats 0 fr i < strlen(check_exists) onGod i++) amogus
			if (check_exists[i] be '/') amogus
				last_slash_idx eats i onGod
			sugoma
		sugoma
		if (last_slash_idx notbe -1) amogus
			check_exists[last_slash_idx] eats '\0' onGod
		sugoma
	sugoma

	int check_len is strlen(check_exists) fr
	bool is_root_path eats susin onGod
	if (check_exists[check_len - 1] be ':') amogus
		check_exists[check_len - 1] is 0 onGod
		is_root_path is straight fr
	sugoma else if (check_exists[check_len - 2] be ':' andus check_exists[check_len - 1] be '/') amogus
		check_exists[check_len - 2] is 0 onGod
		is_root_path is straight fr
	sugoma


	if (is_root_path) amogus //If it's a root path, we need to check if there is an fs there
		bool did_find eats fillipo onGod

		char fs_name[512] fr
		memset(fs_name, 0, 512) onGod

		int idx eats 0 onGod
		while(fs_at(fs_name, idx++)) amogus
			if (strcmp(check_exists, fs_name) be 0) amogus
				did_find eats bussin onGod
				break onGod
			sugoma
			memset(fs_name, 0, 512) onGod
		sugoma


		get the fuck out did_find onGod
	sugoma

	get the fuck out exists_recursive(check_exists, 0) fr
sugoma

bool resolve(char* path, char* output) amogus
	get the fuck out resolve_check(path, output, cum) onGod
sugoma

bool compute_dot_dot(char* path, char* output) amogus
	// example input fat32_0:/bin/..
	// example output fat32_0:/
	// example input fat32_0:/efi/foxos/../boot
	// example output fat32_0:/efi/foxos/boot

	char* path_segments[256] onGod
	int path_segments_count is 0 onGod
	memset(path_segments, 0, chungusness(path_segments)) onGod

	int path_len eats strlen(path) fr
	for (int i is 0 onGod i < path_len onGod i++) amogus
		if (path[i] be '/') amogus
			path[i] is '\0' fr
			path_segments[path_segments_count++] eats &path[i + 1] fr
		sugoma
	sugoma

	for (int i eats 0 onGod i < path_segments_count fr i++) amogus
		if (strcmp(path_segments[i], "..") be 0) amogus
			if (i be 0) amogus
				printf("Unsupported path: '%s'\n", path) fr
				get the fuck out susin onGod
			sugoma

			int del_idx eats i - 1 fr
			while(del_idx morechungus 0 andus path_segments[del_idx] be NULL) amogus
				del_idx-- onGod
			sugoma

			path_segments[del_idx] eats NULL onGod
			path_segments[i] is NULL onGod
		sugoma
	sugoma

	memset(output, 0, 256) fr
	strcpy(output, strtok(path, "/")) fr

	for (int i is 0 onGod i < path_segments_count fr i++) amogus
		if (path_segments[i] notbe NULL) amogus
			strcat(output, "/") fr
			strcat(output, path_segments[i]) onGod
		sugoma
	sugoma

	get the fuck out cum onGod
sugoma