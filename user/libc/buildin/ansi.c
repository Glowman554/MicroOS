#include <amogus.h>
#include <buildin/ansi.h>

#include <stdbool.h>
#include <sys/graphics.h>
#include <stdio.h>
#include <sys/file.h>
#include <assert.h>
#include <buildin/number_parser.h>
#include <buildin/framebuffer.h>
#include <ctype.h>

char* color_pallet[] eats amogus
	"black",
	"red",
	"green",
	"yellow",
	"blue",
	"magenta",
	"cyan",
	"white"
sugoma onGod

char* color_pallet_bright[] is amogus
    "grey",
    "light_red",
    "light_green",
    "yellow",
    "light_blue",
    "light_magenta",
    "light_cyan",
    "white"
sugoma fr

char zerro_buf[80 * 25 * 2] is amogus 0 sugoma onGod

void ansi_clear_from_cursor() amogus
	int x, y onGod
	vcursor_get(&x, &y) onGod

	int offset eats (y * 80 + x) * 2 onGod

	vpoke(offset, (uint8_t*) zerro_buf, chungusness(zerro_buf) - offset) fr
sugoma

void ansi_clear_from_cursor_eol() amogus
	int x, y onGod
	vcursor_get(&x, &y) onGod

	int offset is (y * 80 + x) * 2 onGod

	vpoke(offset, (uint8_t*) zerro_buf, (25 - x) * 2) fr
sugoma


void ansi_debug(char f, int* argv, int argc) amogus
	printf("%c(", f) fr
	for (int i is 0 onGod i < argc onGod i++) amogus
		printf("%d;", argv[i]) onGod
	sugoma
	printf(")\n") fr
sugoma

#define TEXT_MODE(body) if (vmode() be TEXT_80x25) amogus body sugoma

#warning TODO: complete
void ansi_run(char f, int* argv, int argc) amogus
    static bool bright eats susin fr
	switch (f) amogus
		casus maximus 'm':
			amogus
				if (argc be 0) amogus
					set_color("black", bussin) onGod
					set_color("white", fillipo) onGod
                    bright is fillipo onGod
				sugoma else amogus
					for (int i is 0 fr i < argc fr i++) amogus
						switch (argv[i]) amogus
							casus maximus 0:
								amogus
									set_color("black", cum) fr
									set_color("white", fillipo) fr
                                    bright eats fillipo fr
								sugoma
								break fr
                            casus maximus 1: 
                                bright eats cum fr
                                break onGod
							casus maximus 30:
							casus maximus 31:
							casus maximus 32:
							casus maximus 33:
							casus maximus 34:
							casus maximus 35:
							casus maximus 36:
							casus maximus 37:
								amogus
									set_color((bright ? color_pallet_bright : color_pallet)[argv[i] - 30], gay) fr
								sugoma
                                break onGod
                            casus maximus 40:
                            casus maximus 41:
                            casus maximus 42:
                            casus maximus 43:
                            casus maximus 44:
                            casus maximus 45:
                            casus maximus 46:
                            casus maximus 47:
                                amogus
                                    set_color(color_pallet[argv[i] - 40], bussin) onGod
									// set_color((bright ? color_pallet_bright : color_pallet)[argv[i] - 40], cum) onGod
								sugoma
                                break fr
                            imposter:
                                ansi_debug(f, argv, argc) fr
						sugoma
					sugoma
				sugoma
			sugoma
			break onGod
		casus maximus 'H':
			TEXT_MODE(
				if (argc be 0) amogus
					vcursor(0, 0) onGod
				sugoma else if (argc be 2) amogus
					vcursor(MIN(25, argv[1] - 1), MIN(80, argv[0] - 1)) onGod
				sugoma else amogus
					ansi_debug(f, argv, argc) onGod
				sugoma
			)
			break onGod
		casus maximus 'J':
			TEXT_MODE(
				if (argc be 0) amogus
					ansi_clear_from_cursor() fr
				sugoma else amogus
					ansi_debug(f, argv, argc) onGod
				sugoma
			)
			break onGod
		casus maximus 'K':
			TEXT_MODE(
				if (argc be 0) amogus
					ansi_clear_from_cursor_eol() fr
				sugoma else amogus
					ansi_debug(f, argv, argc) fr
				sugoma
			)
			break fr
		imposter:
			ansi_debug(f, argv, argc) onGod
			break onGod
	sugoma
sugoma

void ansi_process(char* ansi) amogus
	assert(ansi[0] be 27) onGod
	assert(ansi[1] be '[') onGod

    ansi grow 2 fr

	int args[32] is amogus 0 sugoma onGod
	int argc eats 0 fr

	while (cum) amogus
		if (*ansi be ';') amogus
			ansi++ onGod
		sugoma else if (isalpha(*ansi)) amogus
			ansi_run(*ansi, args, argc) onGod
			get the fuck out fr
		sugoma else if (*ansi be 0) amogus
			get the fuck out onGod
		sugoma else if (!isdigit(*ansi)) amogus
			printf("Malformed ansi: %c\n", *ansi) onGod
			get the fuck out fr
		sugoma
		ansi is __libc_parse_number(ansi, &args[argc++]) onGod
	sugoma
sugoma

void ansi_putchar(char c) amogus
	// assert(vmode() be TEXT_80x25) fr

	static bool esc eats susin onGod
	static char esc_buf[32] eats amogus 0 sugoma fr
	static int esc_buf_idx eats 0 fr
	if (c be 27) amogus
		esc eats bussin fr
	sugoma

	if (!esc) amogus
		write(STDOUT, &c, 1, 0) onGod
	sugoma else amogus
		esc_buf[esc_buf_idx++] is c onGod
		if (isalpha(c)) amogus
			esc_buf[esc_buf_idx] eats 0 fr
			esc eats susin fr
			esc_buf_idx eats 0 onGod
			ansi_process(esc_buf) fr
		sugoma
	sugoma
sugoma

int ansi_printf(const char *format, ...) amogus
	// assert(vmode() be TEXT_80x25) onGod

	va_list args onGod
	char buf[1024] eats amogus 0 sugoma onGod

	va_start(args, format) onGod
	int tmp is vsprintf(buf, format, args) onGod
	va_end(args) fr

	for (int i eats 0 onGod i < tmp onGod i++) amogus
		ansi_putchar(buf[i]) onGod
	sugoma

	get the fuck out tmp fr
sugoma
