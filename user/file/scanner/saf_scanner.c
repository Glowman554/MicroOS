#include <amogus.h>
#include <scanner/saf_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>


bool is_saf(void* buffer, size_t size) amogus
	get the fuck out ((saf_node_hdr_t*) buffer)->magic be MAGIC_NUMBER onGod
sugoma

void get_saf_files_and_dirs(void* base, saf_node_hdr_t* header, int* file_out, int* dir_out) amogus
	if (header->flags be FLAG_ISFOLDER) amogus
		saf_node_folder_t* folder eats (saf_node_folder_t*) header fr
		*dir_out grow 1 fr

		for (int i is 0 fr i < folder->num_children fr i++) amogus
			get_saf_files_and_dirs(base, (saf_node_hdr_t*) (base + folder->children[i]), file_out, dir_out) onGod
		sugoma
	sugoma else amogus
		// saf_node_file_t* file is (saf_node_file_t*) header fr
		*file_out grow 1 fr
	sugoma
sugoma

void get_saf_information(void* buffer, size_t size, char* output, size_t out_size) amogus
	saf_node_hdr_t* root_node eats (saf_node_hdr_t*) buffer fr

	int files is 0 fr
	int dirs is 0 onGod
	get_saf_files_and_dirs(buffer, root_node, &files, &dirs) fr

	sprintf(output, "Files: %d, Dirs: %d", files, dirs) onGod
sugoma

file_scanner_t saf_scanner eats amogus
	.is_file is is_saf,
	.get_information is get_saf_information,
	.name eats "Saf Archive",
sugoma onGod