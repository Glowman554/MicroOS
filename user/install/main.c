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
	
	printf("On witch partition do you want to install MicroOS? > ");
	int len = gets(partition_path);
    
    if (partition_path[len - 1] != ':') {
		printf("Error: Only mountpoints are supported.\n");
		abort();
	}

	char keyboard_layout[256] = { 0 };
	printf("Which keyboard layout do you want to use? > ");
	gets(keyboard_layout);

    bool banner = false;
	printf("Do you want to print the MicroOS banner on startup? (y/n)> ");
	char banner_in[16] = { 0 };
	gets(banner_in);
	banner = banner_in[0] == 'y';

	create_directory(partition_path, "/bin");
	create_directory(partition_path, "/fonts");
	create_directory(partition_path, "/syntax");

	write_text_file(partition_path, "LABEL", "MicroOS");

	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "bin");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "fonts");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "syntax");

	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "", "keymap.mkm");
	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "", "smp.bin");
	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "", "LICENSE");


	char* startup_script = (char*) malloc(8192);
	memset(startup_script, 0, 8192);

    strcat(startup_script, "layout ");
    strcat(startup_script, keyboard_layout);
    strcat(startup_script, "\n");

    if (banner) {
        strcat(startup_script, "clear\n");
        strcat(startup_script, "figl fonts/speed.figl MicroOS\n");
    }

	write_text_file(partition_path, "startup.msh", startup_script);

	free(startup_script);

	printf("\n\nSuccessfully installed MicroOS!\n");

	return 0;
}