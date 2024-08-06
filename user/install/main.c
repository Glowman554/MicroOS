#include <stdio.h>
#include <stdlib.h>
#include <tools.h>
#include <string.h>

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
	create_directory(partition_path, "/EFI");
	create_directory(partition_path, "/EFI/BOOT");

	write_text_file(partition_path, "/LABEL", "MicroOS");

	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/bin");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/fonts");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/syntax");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/docs");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/EFI/BOOT");

	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/", "keymap.mkm");
	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/", "smp.bin");
	

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
	strcat(limine_config, "MODULE_PATH=boot:///EFI/BOOT/zap-light16.psf\nMODULE_STRING=/zap-light16.psf\n");
	strcat(limine_config, "MODULE_PATH=boot:///EFI/BOOT/mckrnl.syms\nMODULE_STRING=/mckrnl.syms\n");
	strcat(limine_config, "KERNEL_PATH=boot:///EFI/BOOT/mckrnl.elf\n");
	strcat(limine_config, "KERNEL_CMDLINE=--serial --font=/zap-light16.psf --syms=/mckrnl.syms --keymap=MicroOS:/keymap.mkm --init=MicroOS:/bin/init.elf");

	write_text_file(partition_path, "/limine.cfg", limine_config);

	free(startup_script);

	printf("\n\nSuccessfully installed MicroOS!\n");

	return 0;
}