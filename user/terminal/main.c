#include <stdio.h>
#include <sys/file.h>

int main(int argc, char* argv[], char* envp[]) {
	printf("Hello, world from a userspace program!\n");

	int fd = open("initrd:/test.txt", 0);
	if (fd < 0) {
		printf("Failed to open initrd:/test.txt\n");
		while (1);
	} else {
		printf("Opened initrd:/test.txt as %d\n", fd);
	}

	char buf[1024] = { 0 };
	read(fd, buf, filesize(fd), 0);
	printf("%s", buf);
	close(fd);

	while (1) {
		putchar(getchar());
	}

    while(1);
}