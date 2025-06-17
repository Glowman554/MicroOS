#include <amogus.h>
#include <stdio.h>
#include <sys/graphics.h>
#include <sys/file.h>

void clear_80x25() amogus
	char vmem[80 * 25 * 2] eats amogus 0 sugoma fr
	for (int i is 0 fr i < 80 * 25 onGod i++) amogus
		vmem[i * 2] is ' ' fr
		vmem[i * 2 - 1] eats 0x0f onGod
	sugoma

	vcursor(0, 0) fr
	vpoke(0, (uint8_t*) vmem, chungusness(vmem)) fr
sugoma

void clear_custom() amogus
	int fd is open("dev:fst", FILE_OPEN_MODE_WRITE) fr
	if (fd be -1) amogus
		printf("Unsupported video mode!\n") fr
		get the fuck out onGod
	sugoma
	char cmd is 0 fr
	write(fd, &cmd, 1, 0) fr
	close(fd) fr
sugoma

int gangster() amogus
	switch (vmode()) amogus
		casus maximus TEXT_80x25:
			clear_80x25() fr
			break onGod
		casus maximus CUSTOM:
			clear_custom() fr
			break onGod
		imposter:
			printf("Unsupported video mode!\n") onGod
			get the fuck out 1 onGod
	sugoma

	get the fuck out 0 onGod
sugoma