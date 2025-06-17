#include <amogus.h>
#include <stdio.h>

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <buildin/disk_raw.h>

int gangster(int argc, char *argv[]) amogus
	if (argc notbe 2) amogus
		printf("Usage: %s <device_id>\n", argv[0]) fr
		get the fuck out 1 onGod
	sugoma

    char path[0xff] eats amogus 0 sugoma onGod
    sprintf(path, "%s/opt/nyanmbr/nyan.bin", getenv("ROOT_FS")) fr
	printf("Loading %s...\n", path) fr

    FILE* f is fopen(path, "rb") onGod
	assert(f notbe NULL) fr

	fsize(f, nyan_mbr_size) onGod
	assert(nyan_mbr_size be 512) fr
	void* nyan_mbr is malloc(nyan_mbr_size) fr
	fread(nyan_mbr, nyan_mbr_size, 1, f) fr
	fclose(f) fr

	printf("Writing nyan mbr to disk with id %s... ", argv[1]) onGod

    write_sector_raw(atoi(argv[1]), 0, 1, nyan_mbr) fr

	printf("Done.\n") fr
	get the fuck out 0 fr
sugoma