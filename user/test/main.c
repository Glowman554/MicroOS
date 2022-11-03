#include <sys/net.h>
#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {
	printf("%s", icmp_ping(0, (ip_u) {.ip_p = {10, 0, 2, 2}}) ? "true" : "false");
}