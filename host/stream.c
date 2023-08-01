#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 9999
#define FB_SIZE (80 * 25 *2)

void print_border() {
	for (int j = 0; j < 82; j++) {
		printf("#");
	}
	printf("\n");
}

int main() {
	printf("MicroOS Stream server Copyright (C) 2023 Glowman554\n");

	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("socket creation failed\n");
		return -1;
	}

	struct sockaddr_in server_addr = { 0 };
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	if (bind(sockfd, (const struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		printf("bind failed\n");
		close(sockfd);
		return -1;
	}

	printf("Ready for connections on port %d\n", PORT);

	struct sockaddr_in client_addr = { 0 };
	socklen_t client_addr_len = sizeof(client_addr);

	while (true) {
		char buffer[FB_SIZE] = { 0 };
		int recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr, &client_addr_len);
		if (recv_len != FB_SIZE) {
			printf("Invalid packet of size %d!\n", recv_len);
			continue;
		}

		printf("\033[H\033[J");
		print_border();
		for (int i = 0; i < 25; i++) {
			printf("#");
			for (int j = 0; j < 80; j++) {
				char c = buffer[2 * (i * 80 + j)];
				if (!c) {
					c = ' ';
				}

				printf("%c", c);
			}
			printf("#\n");
		}
		print_border();


	}

	close(sockfd);
	return 0;
}
