#include <buildin/ansi.h>

#include <stdbool.h>
#include <sys/graphics.h>
#include <stdio.h>
#include <sys/file.h>
#include <assert.h>
#include <buildin/number_parser.h>
#include <buildin/framebuffer.h>
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

char* color_pallet_bright[] = {
    "grey",
    "light_red",
    "light_green",
    "yellow",
    "light_blue",
    "light_magenta",
    "light_cyan",
    "white"
};

char zerro_buf[80 * 25 * 2] = { 0 };

void ansi_clear_from_cursor() {
	int x, y;
	vcursor_get(&x, &y);

	int offset = (y * 80 + x) * 2;

	vpoke(offset, (uint8_t*) zerro_buf, sizeof(zerro_buf) - offset);
}

void ansi_clear_from_cursor_eol() {
	int x, y;
	vcursor_get(&x, &y);

	int offset = (y * 80 + x) * 2;

	vpoke(offset, (uint8_t*) zerro_buf, (25 - x) * 2);
}


void ansi_debug(char f, int* argv, int argc) {
	printf("%c(", f);
	for (int i = 0; i < argc; i++) {
		printf("%d;", argv[i]);
	}
	printf(")\n");
}

#define TEXT_MODE(body) if (vmode() == TEXT_80x25) { body }

#warning TODO: complete
void ansi_run(char f, int* argv, int argc) {
    static bool bright = false;
	switch (f) {
		case 'm':
			{
				if (argc == 0) {
					set_color("black", true);
					set_color("white", false);
                    bright = false;
				} else {
					for (int i = 0; i < argc; i++) {
						switch (argv[i]) {
							case 0:
								{
									set_color("black", true);
									set_color("white", false);
                                    bright = false;
								}
								break;
                            case 1: 
                                bright = true;
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
									set_color((bright ? color_pallet_bright : color_pallet)[argv[i] - 30], false);
								}
                                break;
                            case 40:
                            case 41:
                            case 42:
                            case 43:
                            case 44:
                            case 45:
                            case 46:
                            case 47:
                                {
                                    set_color(color_pallet[argv[i] - 40], true);
									// set_color((bright ? color_pallet_bright : color_pallet)[argv[i] - 40], true);
								}
                                break;
                            default:
                                ansi_debug(f, argv, argc);
						}
					}
				}
			}
			break;
		case 'H':
			TEXT_MODE(
				if (argc == 0) {
					vcursor(0, 0);
				} else if (argc == 2) {
					vcursor(MIN(25, argv[1] - 1), MIN(80, argv[0] - 1));
				} else {
					ansi_debug(f, argv, argc);
				}
			)
			break;
		case 'J':
			TEXT_MODE(
				if (argc == 0) {
					ansi_clear_from_cursor();
				} else {
					ansi_debug(f, argv, argc);
				}
			)
			break;
		case 'K':
			TEXT_MODE(
				if (argc == 0) {
					ansi_clear_from_cursor_eol();
				} else {
					ansi_debug(f, argv, argc);
				}
			)
			break;
		default:
			ansi_debug(f, argv, argc);
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
		} else if (!isdigit(*ansi)) {
			printf("Malformed ansi: %c\n", *ansi);
			return;
		}
		ansi = __libc_parse_number(ansi, &args[argc++]);
	}
}

void ansi_putchar(char c) {
	// assert(vmode() == TEXT_80x25);

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
	// assert(vmode() == TEXT_80x25);

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
