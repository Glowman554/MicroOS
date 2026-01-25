#include <net/http.h>

#include <buildin/time.h>

#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {
	if (argc != 3) {
		printf("Usage: %s <host> <path>\n", argv[0]);
		printf("Example: %s toxicfox.de /\n", argv[0]);
		return 1;
	}

	int nic = 0;
	http_request_t* request = http_request_create("GET", argv[1], argv[2]);

	int status = http_request_perform(nic, request);
	
	printf("HTTP Response Code: %d\n", status);
	printf("Response Body (%d bytes):\n", request->response_body_length);
	
	printf("Headers:\n");
	for (int i = 0; i < request->response_headers_count; i++) {
		printf("  %s\n", request->response_headers[i]);
	}

	printf("\nBody:\n");

	for (int i = 0; i < request->response_body_length; i++) {
		putchar(request->response_body[i]);
	}

	http_request_free(request);
	return 0;
}