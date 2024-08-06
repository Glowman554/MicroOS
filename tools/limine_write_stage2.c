#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "limine-hdd.h"


int main(int argc, char* argv[], char* envp[]) {
	const uint8_t* bootloader_img = binary_limine_hdd_bin_data;
	size_t bootloader_file_size = sizeof(binary_limine_hdd_bin_data);

    FILE* f = fopen("limine_hdd.bin", "wb");
    if (!f) {
        printf("Could not open output file!\n");
        return -1;
    }
    fwrite(bootloader_img, bootloader_file_size, 1, f);
    fclose(f);
}