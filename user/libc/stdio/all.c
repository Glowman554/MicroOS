#include <stdio.h>
#include <stdlib.h>
#include <sys/getc.h>

void read_all_stdin(char** out, size_t* size) {
	const size_t chunk = 4096;
	size_t cap = chunk;
	size_t len = 0;
	char* buf = malloc(cap);

	while (1) {
        char c = async_getc();
        if (c == EOF_CHAR) {
            break;
        }

        if (len + 1 >= cap) {
            cap += chunk;
            buf = realloc(buf, cap);
        }

        buf[len] = c;
        len++;
	}

	*out = buf;
	*size = len;
}

void read_all_file(FILE* f, char** out, size_t* size) {
    fsize(f, file_size);

    char* buf = malloc(file_size);
    fread(buf, file_size, 1, f);

    *out = buf;
    *size = file_size;
}