#include <logo.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <buildin/unix_time.h>
#include <sys/time.h>
#include <sys/raminfo.h>
#include <sys/file.h>
#include <buildin/disk_raw.h>

#include <sys/graphics.h>
#define FB_SET_PX_IMPL
#include <buildin/framebuffer.h>



#define is_kb(x) ((x) >= 1024)
#define is_mb(x) ((x) >= 1024 * 1024)
#define is_gb(x) ((x) >= 1024 * 1024 * 1024)

#define to_kb(x) ((x) / 1024)
#define to_mb(x) ((x) / 1024 / 1024)
#define to_gb(x) ((x) / 1024 / 1024 / 1024)


void format_memory_usage(uint32_t ussage) {
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









void print_time() {
    long unix_time = time(NULL);
    char date[128] = { 0 };
    unix_time_to_string(unix_time, date);

    printf("Time: %s", date);
}

void print_memory() {
    uint32_t free;
    uint32_t used;
    raminfo(&free, &used);
    printf("RAM: ");
    format_memory_usage(used );
    printf(" / ");
    format_memory_usage(free + used);
}


void print_nic() {
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

    printf("Network interfaces: %d", nic_count);
}

void print_disk() {
    int num_disks = disk_count(NULL);
	bool physical[num_disks];
	disk_count(physical);

    int count = 0;
    int count_physical = 0; 

	for (int i = 0; i < num_disks; i++) {
		if (physical[i]) {
            count_physical++;
		}
        count++;
	}

    printf("Disks: %d (%d physical)", count, count_physical);
}

void print_filesystems() {
	char out[512];
	memset(out, 0, 512);

	int idx = 0;
	while(fs_at(out, idx++)) {
		memset(out, 0, 512);
	}

    printf("Mounted filesystems: %d", idx - 1);
}

typedef void (*line)();
line lines[] = { print_time, print_memory, print_nic, print_disk, print_filesystems };

void print_logo() {

    if (vmode() == CUSTOM) {
        fb_info_t info = fb_load_info();

        for (int i = 0; i < logo_img.width; i++) {
            for (int j = 0; j < logo_img.height; j++) {
                uint32_t color = logo_img.pixels[j * logo_img.width + i];
                fb_set_pixel(&info, i, j, color);
            }
        }


        for (int i = 0; i < sizeof(lines) / sizeof(line); i++) {
            vcursor(logo_img.width / 8, i);
            lines[i]();
        }

        vcursor(0, logo_img.height / 16);

    } else {
        int linenum = 0;
        for (int i = 0; i < logo_txt_size; i++) {
            if (logo_txt[i] == '\n') {
                puts("  ");
                
                if (linenum < sizeof(lines) / sizeof(line)) {
                    lines[linenum++]();
                }

                putchar('\n');
            } else {
                putchar(logo_txt[i]);
            }
        }
    }
}