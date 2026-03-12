#include <non-standart/stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syntax.h>
#include <non-standart/sys/graphics.h>

char* color_lut[] = {
    [black]   = "black",
    [red]     = "red",
    [green]   = "green",
    [yellow]  = "yellow",
    [blue]    = "blue",
    [magenta] = "magenta",
    [cyan]    = "cyan",
    [white]   = "white",
};

char* get_file_extension(const char* filename) {
	char* chr_ptr = strchr(filename, '.');
	if (chr_ptr == NULL) {
		return "";
	}
	return ++chr_ptr;
}


int main(int argc, char *argv[]) {
	char* buffer = NULL;
	size_t size = 0;

	if (argc == 2) {
		FILE* fp = fopen(argv[1], "r");
		if (fp == NULL) {
			printf("Error: No such file or directory: %s\n", argv[1]);
			return 1;
		}
		read_all_file(fp, &buffer, &size);
	} else {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

    char syx[128] = { 0 };
	strcat(syx, getenv("ROOT_FS"));
	strcat(syx, "syntax/");
	strcat(syx, get_file_extension(argv[1]));
	strcat(syx, ".syx");
	syntax_header_t* syntax = load_syntax(syx);
    if (!syntax) {
        printf("No syntax definition found for .%s files.\n", get_file_extension(argv[1]));
    }

    uint8_t* color = highlight(buffer, size, syntax);

    char* prev = NULL;
	for (size_t i = 0; i < size; i++) {
        if (prev != color_lut[color[i]]) {
            set_color(color_lut[color[i]], false);
            prev = color_lut[color[i]];
        }
        
		printf("%c", buffer[i]);
	}

    set_color("white", false);

	free(buffer);
	return 0;
}