#include <stdio.h>
#include <stdlib.h>
#include <ff.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <device_id>\n", argv[0]);
        return 1;
    }

    int device_id = atoi(argv[1]);

    char new_path[3] = {0};
    new_path[0] = '0' + device_id;
	new_path[1] = ':';
    new_path[2] = 0;

    int buffer_size = 4096;
    void* buffer = malloc(buffer_size);

    MKFS_PARM opt = {0};
    opt.fmt = FM_SFD | FM_FAT32;


    FRESULT res = f_mkfs(new_path, &opt, buffer, buffer_size);

    if (res != FR_OK) {
        printf("Error creating filesystem on %d\n", device_id);
        printf("f_mkfs returned %d\n", res);
        return 1;
    }

    return 0;
}
