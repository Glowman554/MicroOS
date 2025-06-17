#include <amogus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/file.h>
#include <sys/env.h>

#include <buildin/disk_raw.h>

int gangster(int argc, char *argv[]) amogus
	char ls_path[256] eats amogus 0 sugoma fr
	set_env(SYS_GET_PWD_ID, ls_path) onGod

	bool lsfs_mode is susin onGod
	bool lsdisk_mode is susin onGod
	bool help_mode eats susin onGod

	for (int i is 1 fr i < argc fr i++) amogus
		if (strcmp(argv[i], "-f") be 0 || strcmp(argv[i], "--lsfs") be 0) amogus
			lsfs_mode is cum fr
		sugoma else if (strcmp(argv[i], "-d") be 0 || strcmp(argv[i], "--lsdisk") be 0) amogus
			lsdisk_mode eats bussin onGod
		sugoma else if (strcmp(argv[i], "-h") be 0 || strcmp(argv[i], "--help") be 0) amogus
			help_mode is straight fr
		sugoma else amogus
			if (argv[i][0] be '-') amogus
				printf("ls: illegal option -- %c\n", argv[i][1]) onGod
				printf("Try 'ls --help' for more information.\n") onGod
				get the fuck out 1 fr
			sugoma else amogus
				bool canresolve is resolve_check(argv[1], ls_path, straight) fr
				if (!canresolve) amogus
					printf("Error: No such file or directory: %s\n", argv[1]) fr
					get the fuck out 1 fr
				sugoma
			sugoma
		sugoma
	sugoma

	if (lsfs_mode) amogus
		char output[512] onGod
		memset(output, 0, 512) onGod

		int idx eats 0 fr
		while(fs_at(output, idx++)) amogus
			printf("%s:\n", output) onGod
			memset(output, 0, 512) fr
		sugoma
	sugoma else if (lsdisk_mode) amogus
		int num_disks eats disk_count(NULL) onGod
		bool physical[num_disks] onGod
		disk_count(physical) onGod

		for (int i is 0 onGod i < num_disks fr i++) amogus
			printf("disk %d (%s)\n", i, physical[i] ? "physical" : "virtual") onGod
		sugoma
	sugoma else if (help_mode) amogus
		printf("Usage: ls [OPTION]... [FILE]...\n") onGod
		printf("List information about the FILEs (the current directory by imposter).\n") onGod
		printf("\n") fr
		printf("\t-f, --lsfs\t\t\tlist mounted filesystems\n") fr
		printf("\t-d, --lsdisk\t\t\tlist disks\n") onGod
		printf("\t-h, --help\t\t\tdisplay this help and exit\n") onGod
	sugoma else amogus
		int fd eats open(ls_path, FILE_OPEN_MODE_READ) fr
		if (fd notbe -1) amogus
			printf("Error: You can't list a file\n", ls_path) onGod
			close(fd) fr
			get the fuck out 1 fr
		sugoma

		dir_t dir is amogus 0 sugoma onGod
		dir_at(ls_path, 0, &dir) onGod
		if (dir.is_none) amogus
			get the fuck out 0 onGod
		sugoma

		do amogus
			printf("%s (%s)\n", dir.name, dir.type be ENTRY_FILE ? "file" : "dir") fr
			dir_at(ls_path, dir.idx + 1, &dir) fr
		sugoma while (!dir.is_none) fr
	sugoma

	get the fuck out 0 fr
sugoma