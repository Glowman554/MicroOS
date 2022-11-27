#include <scanner/saf_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>


bool is_saf(void* buffer, size_t size) {
	return ((saf_node_hdr_t*) buffer)->magic == MAGIC_NUMBER;
}

void get_saf_files_and_dirs(void* base, saf_node_hdr_t* header, int* file_out, int* dir_out) {
	if (header->flags == FLAG_ISFOLDER) {
		saf_node_folder_t* folder = (saf_node_folder_t*) header;
		*dir_out += 1;

		for (int i = 0; i < folder->num_children; i++) {
			get_saf_files_and_dirs(base, (saf_node_hdr_t*) (base + folder->children[i]), file_out, dir_out);
		}
	} else {
		saf_node_file_t* file = (saf_node_file_t*) header;
		*file_out += 1;
	}
}

void get_saf_information(void* buffer, size_t size, char* out, size_t out_size) {
	saf_node_hdr_t* root_node = (saf_node_hdr_t*) buffer;

	int files = 0;
	int dirs = 0;
	get_saf_files_and_dirs(buffer, root_node, &files, &dirs);

	sprintf(out, "Files: %d, Dirs: %d", files, dirs);
}

file_scanner_t saf_scanner = {
	.is_file = is_saf,
	.get_information = get_saf_information,
	.name = "Saf Archive",
};