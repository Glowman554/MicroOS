#include <stdio.h>

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <buildin/disk_raw.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <device_id>\n", argv[0]);
		return 1;
	}

    char path[0xff] = { 0 };
    sprintf(path, "%s/opt/nyanmbr/nyan.bin", getenv("ROOT_FS"));
	printf("Loading %s...\n", path);

    FILE* f = fopen(path, "rb");
	assert(f != NULL);

	fsize(f, nyan_mbr_size);
	assert(nyan_mbr_size == 512);
	void* nyan_mbr = malloc(nyan_mbr_size);
	fread(nyan_mbr, nyan_mbr_size, 1, f);
	fclose(f);

	printf("Writing nyan mbr to disk with id %s... ", argv[1]);

    write_sector_raw(atoi(argv[1]), 0, 1, nyan_mbr);

	printf("Done.\n");
	return 0;
}