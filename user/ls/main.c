#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/file.h>
#include <sys/env.h>

#include <buildin/disk_raw.h>

int main(int argc, char *argv[]) {
	char ls_path[256] = {0};
	set_env(SYS_GET_PWD_ID, ls_path);

	bool lsfs_mode = false;
	bool lsdisk_mode = false;
	bool help_mode = false;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--lsfs") == 0) {
			lsfs_mode = true;
		} else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--lsdisk") == 0) {
			lsdisk_mode = true;
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			help_mode = true;
		} else {
			if (argv[i][0] == '-') {
				printf("ls: illegal option -- %c\n", argv[i][1]);
				printf("Try 'ls --help' for more information.\n");
				return 1;
			} else {
				bool canresolve = resolve_check(argv[1], ls_path, true);
				if (!canresolve) {
					printf("Error: No such file or directory: %s\n", argv[1]);
					return 1;
				}
			}
		}
	}

	if (lsfs_mode) {
		char out[512];
		memset(out, 0, 512);

		int idx = 0;
		while(fs_at(out, idx++)) {
			printf("%s:\n", out);
			memset(out, 0, 512);
		}
	} else if (lsdisk_mode) {
		int num_disks = disk_count(NULL);
		bool physical[num_disks];
		disk_count(physical);

		for (int i = 0; i < num_disks; i++) {
			printf("disk %d (%s)\n", i, physical[i] ? "physical" : "virtual");
		}
	} else if (help_mode) {
		printf("Usage: ls [OPTION]... [FILE]...\n");
		printf("List information about the FILEs (the current directory by default).\n");
		printf("\n");
		printf("\t-f, --lsfs\t\t\tlist mounted filesystems\n");
		printf("\t-d, --lsdisk\t\t\tlist disks\n");
		printf("\t-h, --help\t\t\tdisplay this help and exit\n");
	} else {
		int fd = open(ls_path, FILE_OPEN_MODE_READ);
		if (fd != -1) {
			printf("Error: You can't list a file\n", ls_path);
			close(fd);
			return 1;
		}

		dir_t dir = {0};
		dir_at(ls_path, 0, &dir);
		if (dir.is_none) {
			return 0;
		}

		do {
			printf("%s (%s)\n", dir.name, dir.type == ENTRY_FILE ? "file" : "dir");
			dir_at(ls_path, dir.idx + 1, &dir);
		} while (!dir.is_none);
	}

	return 0;
}