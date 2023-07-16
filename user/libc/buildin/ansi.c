#include <buildin/ansi.h>

#include <stdbool.h>
#include <sys/graphics.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/file.h>
#include <assert.h>
#include <buildin/number_parser.h>
#include <ctype.h>

char* color_pallet[] = {
	"black",
	"red",
	"green",
	"yellow",
	"blue",
	"magenta",
	"cyan",
	"white"
};

#warning TODO: complete
void ansi_run(char f, int* argv, int argc) {
	switch (f) {
		case 'm':
			{
				if (argc == 0) {
					set_color("black", true);
					set_color("white", false);
				}
				for (int i = 0; i < argc; i++) {
					switch (argv[i]) {
						case 0:
							{
								set_color("black", true);
								set_color("white", false);
							}
							break;
						case 30:
						case 31:
						case 32:
						case 33:
						case 34:
						case 35:
						case 36:
						case 37:
							{
								set_color(color_pallet[argv[i] - 30], false);
							}
					}
				}
			}
			break;
	}
}

void ansi_process(char* ansi) {
	assert(ansi[0] == 27);
	assert(ansi[1] == '[');

    ansi += 2;

	int args[32] = { 0 };
	int argc = 0;

	while (true) {
		if (*ansi == ';') {
			ansi++;
		} else if (isalpha(*ansi)) {
			ansi_run(*ansi, args, argc);
			return;
		} else if (*ansi == 0) {
			return;
		}
		ansi = __libc_parse_number(ansi, &args[argc++]);
	}
}

void ansi_putchar(char c) {
	static bool esc = false;
	static char esc_buf[32] = { 0 };
	static int esc_buf_idx = 0;
	if (c == 27) {
		esc = true;
	}

	if (!esc) {
		write(STDOUT, &c, 1, 0);
	} else {
		esc_buf[esc_buf_idx++] = c;
		if (isalpha(c)) {
			esc_buf[esc_buf_idx] = 0;
			esc = false;
			esc_buf_idx = 0;
			ansi_process(esc_buf);
		}
	}
}

int ansi_printf(const char *format, ...) {
	va_list args;
	char buf[1024] = {0};

	va_start(args, format);
	int tmp = vsprintf(buf, format, args);
	va_end(args);

	for (int i = 0; i < tmp; i++) {
		ansi_putchar(buf[i]);
	}

	return tmp;
}
