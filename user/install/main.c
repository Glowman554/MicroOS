#include <stdio.h>
#include <stdlib.h>
#include <tools.h>
#include <string.h>
#include <sys/file.h>
#include <saf.h>

void pack_kernel_modules(const char *partition_path) {
	char modules_path[512];
	sprintf(modules_path, "%s/modules", partition_path);

	int file_count = 0;
	dir_t dir;

	for (dir_at(modules_path, 0, &dir); !dir.is_none; dir_at(modules_path, dir.idx + 1, &dir)) {
		if (dir.type == ENTRY_FILE) {
			file_count++;
		}
	}

	size_t alloc_size = sizeof(saf_node_folder_t) + file_count * sizeof(saf_offset_t);

	for (dir_at(modules_path, 0, &dir); !dir.is_none; dir_at(modules_path, dir.idx + 1, &dir)) {
		if (dir.type != ENTRY_FILE) {
			continue;
		}

		char file_path[1024];
		sprintf(file_path, "%s/%s", modules_path, dir.name);

		FILE *f = fopen(file_path, "rb");
		if (!f) {
			printf("Error: cannot open %s\n", file_path);
			abort();
		}

		fseek(f, 0, SEEK_END);
		alloc_size += sizeof(saf_node_file_t) + ftell(f);
		fclose(f);
	}

	uint8_t *saf_data = calloc(1, alloc_size);

	size_t curr_offset = 0;

	saf_node_folder_t *root = (saf_node_folder_t *)(saf_data + curr_offset);

	root->hdr.magic = MAGIC_NUMBER;
	root->hdr.flags = FLAG_ISFOLDER;
	root->num_children = file_count;
	strcpy(root->hdr.name, "modules");

	root->hdr.len = sizeof(saf_node_folder_t) + file_count * sizeof(saf_offset_t);

	curr_offset += root->hdr.len;

	int child_idx = 0;

	for (dir_at(modules_path, 0, &dir); !dir.is_none; dir_at(modules_path, dir.idx + 1, &dir)) {
		if (dir.type != ENTRY_FILE) {
			continue;
		}

		char file_path[1024];
		sprintf(file_path, "%s/%s", modules_path, dir.name);

		FILE *f = fopen(file_path, "rb");
		if (!f) {
			printf("Error: cannot open %s\n", file_path);
			abort();
		}

		fseek(f, 0, SEEK_END);
		size_t file_size = ftell(f);
		fseek(f, 0, SEEK_SET);

		saf_node_file_t *file_node = (saf_node_file_t *)(saf_data + curr_offset);

		file_node->hdr.magic = MAGIC_NUMBER;
		file_node->hdr.flags = 0;
		file_node->hdr.len   = sizeof(saf_node_file_t);
		strcpy(file_node->hdr.name, dir.name);

		file_node->size = file_size;
		file_node->addr = curr_offset + sizeof(saf_node_file_t);

		fread((uint8_t *)file_node + sizeof(saf_node_file_t), 1, file_size, f);
		fclose(f);

		root->children[child_idx++] = curr_offset;
		curr_offset += sizeof(saf_node_file_t) + file_size;
	}

	char saf_output_path[512];
	sprintf(saf_output_path, "%s/modules.saf", partition_path);

	FILE *out = fopen(saf_output_path, "wb");
	if (!out) {
		printf("Error: cannot write SAF archive\n");
		abort();
	}

	fwrite(saf_data, 1, curr_offset, out);
	fclose(out);
	free(saf_data);

	printf("[PACK]  Packed %d kernel modules into %s\n", file_count, saf_output_path);
}

int main(int argc, char* argv[]) {
	if (argc != 1 && argc != 4) {
		printf("Usage: %s\n", argv[0]);
		return -1;
	}

	printf("Welcome to the MicroOS installer!\n");
	char partition_path[256] = { 0 };
	
	if (getenv("partition")) {
		strcpy(partition_path, getenv("partition"));
	} else {
		printf("On witch partition do you want to install MicroOS? > ");
		int len = gets(partition_path);
		
		if (partition_path[len - 1] != ':') {
			printf("Error: Only mountpoints are supported.\n");
			abort();
		}
	}

	char keyboard_layout[256] = { 0 };
	if (getenv("keyboard")) {
		strcpy(keyboard_layout, getenv("keyboard"));
	} else {
		printf("Which keyboard layout do you want to use? > ");
		gets(keyboard_layout);
	}

    bool banner = false;
	if (getenv("banner")) {
		banner = getenv("banner")[0] == 'y';
	} else {
		printf("Do you want to print the MicroOS banner on startup? (y/n)> ");
		char banner_in[16] = { 0 };
		gets(banner_in);
		banner = banner_in[0] == 'y';
	}

	int init_terms = 0;
	if (getenv("terms")) {
		init_terms = atoi(getenv("terms"));
	} else {
		printf("How many extra terminals do you want to start? > ");
		char terms_in[16] = { 0 };
		gets(terms_in);
		init_terms = atoi(terms_in);
	}

	create_directory(partition_path, "/bin");
	create_directory(partition_path, "/fonts");
	create_directory(partition_path, "/syntax");
	create_directory(partition_path, "/docs");
	create_directory(partition_path, "/opt");
	create_directory(partition_path, "/EFI");
	create_directory(partition_path, "/EFI/BOOT");
	create_directory(partition_path, "modules");

	write_text_file(partition_path, "/LABEL", "MicroOS");

	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/bin");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/fonts");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/syntax");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/docs");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/opt");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/EFI/BOOT");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/modules");

	pack_kernel_modules(partition_path);

	// copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/", "keymap.mkm");
	// copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/", "smp.bin");
	// copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/", "idle.bin");
	

	char* startup_script = (char*) malloc(8192);
	memset(startup_script, 0, 8192);

    strcat(startup_script, "layout ");
    strcat(startup_script, keyboard_layout);
    strcat(startup_script, "\n");

	for (int i = 0; i < init_terms; i++) {
        strcat(startup_script, "background ");
		char buf[6] = { 0 };
		sprintf(buf, "%d", i + 2);
        strcat(startup_script, buf);
        strcat(startup_script, " terminal\n");
	}

    if (banner) {
        strcat(startup_script, "clear\n");
        strcat(startup_script, "figl fonts/speed.figl MicroOS\n");
    }

	write_text_file(partition_path, "/startup.msh", startup_script);

	char* limine_config = (char*) malloc(8192);
	memset(limine_config, 0, 8192);

	strcat(limine_config, "TIMEOUT 3\n");
	strcat(limine_config, ":MicroOS\n");
	strcat(limine_config, "KASLR=no\n");
	strcat(limine_config, "PROTOCOL=multiboot1\n");
	strcat(limine_config, "MODULE_PATH=boot:///EFI/BOOT/mckrnl.syms\nMODULE_STRING=/mckrnl.syms\n");
	strcat(limine_config, "MODULE_PATH=boot:///modules.saf\nMODULE_STRING=/modules.saf\n");
	strcat(limine_config, "KERNEL_PATH=boot:///EFI/BOOT/mckrnl.elf\n");
	strcat(limine_config, "KERNEL_CMDLINE=--serial --syms=/mckrnl.syms --modules=/modules.saf:/ --init=MicroOS:/bin/init.mex");

	write_text_file(partition_path, "/limine.cfg", limine_config);

	free(startup_script);

	printf("\n\nSuccessfully installed MicroOS!\n");

	return 0;
}