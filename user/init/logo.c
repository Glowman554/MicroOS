#include <logo.h>

#include <stdio.h>
#include <string.h>

#include <buildin/unix_time.h>
#include <sys/time.h>
#include <sys/raminfo.h>
#include <sys/file.h>
#include <buildin/disk_raw.h>


#define is_kb(x) ((x) >= 1024)
#define is_mb(x) ((x) >= 1024 * 1024)
#define is_gb(x) ((x) >= 1024 * 1024 * 1024)

#define to_kb(x) ((x) / 1024)
#define to_mb(x) ((x) / 1024 / 1024)
#define to_gb(x) ((x) / 1024 / 1024 / 1024)

void format_memory_ussage(uint32_t ussage) {
	if (is_gb(ussage)) {
		printf("%d,%d GB", to_gb(ussage), to_mb(ussage) % 1024);
	} else if (is_mb(ussage)) {
		printf("%d,%d MB", to_mb(ussage), to_kb(ussage) % 1024);
	} else if (is_kb(ussage)) {
		printf("%d,%d KB", to_kb(ussage), ussage % 1024);
	} else {
		printf("%d B", ussage);
	}
}

int count_nic() {
    int nic_count = 0;
    while (true) {
        char nic_path[256] = { 0 };
        sprintf(nic_path, "dev:nic%d", nic_count);
        int fd = open(nic_path, FILE_OPEN_MODE_READ);
        if (fd == -1) {
            break;
        }
        close(fd);
        nic_count++;
    }

    return nic_count;
}

int count_disks(bool physical_only) {
    int num_disks = disk_count(NULL);
	bool physical[num_disks];
	disk_count(physical);

    int count = 0;

	for (int i = 0; i < num_disks; i++) {
		if (!physical_only || physical[i]) {
            count++;
		}
	}

    return count;
}

int count_filesystems() {
	char out[512];
	memset(out, 0, 512);

	int idx = 0;
	while(fs_at(out, idx++)) {
		memset(out, 0, 512);
	}

    return idx - 1;
}

void print_logo() {
    int linenum = 0;
    for (int i = 0; i < logo_txt_size; i++) {
        if (logo_txt[i] == '\n') {
            puts("  ");
            switch (linenum++) {
                case 0:
                    {
		                long unix_time = time(NULL);
                        char date[128] = { 0 };
                        unix_time_to_string(unix_time, date);

                        printf("Time: %s", date);
                    }
                    break;
                case 1:
                    {
                        uint32_t free;
                        uint32_t used;
                        raminfo(&free, &used);
                        printf("RAM: ");
                        format_memory_ussage(used );
                        printf(" / ");
                        format_memory_ussage(free + used);
                    }
                    break;
                case 2:
                    printf("Network interfaces: %d", count_nic());
                    break;
                case 3:
                    printf("Disks: %d (%d physical)", count_disks(false), count_disks(true));
                    break;
                case 4:
                    printf("Mounted filesystems: %d", count_filesystems());
                    break;
            }

            putchar('\n');
        } else {
            putchar(logo_txt[i]);
        }
    }
}