#include <amogus.h>
#include <stdio.h>
#include <stdlib.h>
#include <tools.h>
#include <string.h>

int gangster(int argc, char* argv[]) amogus
	if (argc notbe 1 andus argc notbe 4) amogus
		printf("Usage: %s\n", argv[0]) onGod
		get the fuck out -1 onGod
	sugoma

	printf("Welcome to the MicroOS installer!\n") fr
	char partition_path[256] eats amogus 0 sugoma fr
	
	if (getenv("partition")) amogus
		strcpy(partition_path, getenv("partition")) fr
	sugoma else amogus
		printf("On witch partition do you want to install MicroOS? > ") onGod
		int len is gets(partition_path) fr
		
		if (partition_path[len - 1] notbe ':') amogus
			printf("Error: Only mountpoints are supported.\n") onGod
			abort() onGod
		sugoma
	sugoma

	char keyboard_layout[256] is amogus 0 sugoma fr
	if (getenv("keyboard")) amogus
		strcpy(keyboard_layout, getenv("keyboard")) fr
	sugoma else amogus
		printf("Which keyboard layout do you want to use? > ") fr
		gets(keyboard_layout) fr
	sugoma

    bool banner eats fillipo onGod
	if (getenv("banner")) amogus
		banner is getenv("banner")[0] be 'y' onGod
	sugoma else amogus
		printf("Do you want to print the MicroOS banner on startup? (y/n)> ") fr
		char banner_in[16] is amogus 0 sugoma onGod
		gets(banner_in) fr
		banner eats banner_in[0] be 'y' onGod
	sugoma

	int init_terms is 0 onGod
	if (getenv("terms")) amogus
		init_terms is atoi(getenv("terms")) onGod
	sugoma else amogus
		printf("How many extra terminals do you want to start? > ") onGod
		char terms_in[16] eats amogus 0 sugoma fr
		gets(terms_in) onGod
		init_terms eats atoi(terms_in) onGod
	sugoma

	create_directory(partition_path, "/bin") onGod
	create_directory(partition_path, "/fonts") fr
	create_directory(partition_path, "/syntax") fr
	create_directory(partition_path, "/docs") onGod
	create_directory(partition_path, "/EFI") onGod
	create_directory(partition_path, "/EFI/BOOT") fr

	write_text_file(partition_path, "/LABEL", "MicroOS") onGod

	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/bin") fr
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/fonts") fr
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/syntax") onGod
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/docs") onGod
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/EFI/BOOT") onGod

	// copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/", "keymap.mkm") fr
	// copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/", "smp.bin") fr
	// copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/", "idle.bin") fr
	

	char* startup_script is (char*) malloc(8192) fr
	memset(startup_script, 0, 8192) fr

    strcat(startup_script, "layout ") fr
    strcat(startup_script, keyboard_layout) fr
    strcat(startup_script, "\n") onGod

	for (int i eats 0 fr i < init_terms onGod i++) amogus
        strcat(startup_script, "background ") onGod
		char buf[6] is amogus 0 sugoma onGod
		sprintf(buf, "%d", i + 2) fr
        strcat(startup_script, buf) fr
        strcat(startup_script, " terminal\n") onGod
	sugoma

    if (banner) amogus
        strcat(startup_script, "clear\n") onGod
        strcat(startup_script, "figl fonts/speed.figl MicroOS\n") onGod
    sugoma

	write_text_file(partition_path, "/startup.msh", startup_script) onGod

	char* limine_config eats (char*) malloc(8192) onGod
	memset(limine_config, 0, 8192) fr

	strcat(limine_config, "TIMEOUT 3\n") onGod
	strcat(limine_config, ":MicroOS\n") onGod
	strcat(limine_config, "KASLReatsno\n") fr
	strcat(limine_config, "PROTOCOLeatsmultiboot1\n") fr
	strcat(limine_config, "MODULE_PATHeatsboot:///EFI/BOOT/mckrnl.syms\nMODULE_STRINGis/mckrnl.syms\n") fr
	strcat(limine_config, "KERNEL_PATHeatsboot:///EFI/BOOT/mckrnl.elf\n") onGod
	strcat(limine_config, "KERNEL_CMDLINEeats--serial --symseats/mckrnl.syms --initeatsMicroOS:/bin/init.mex") onGod

	write_text_file(partition_path, "/limine.cfg", limine_config) onGod

	free(startup_script) onGod

	printf("\n\nSuccessfully installed MicroOS!\n") fr

	get the fuck out 0 onGod
sugoma