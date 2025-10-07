#include <window/vconsole.h>

#include <assert.h>
#include <buildin/number_parser.h>
#include <ctype.h>

#include <window.h>

void vconsole_ansi_debug(char f, int* argv, int argc) {
	// printf("%c(", f);
	for (int i = 0; i < argc; i++) {
		// printf("%d;", argv[i]);
	}
	// printf(")\n");
}

uint32_t vconsole_ansi_color_pallet[] = {
	0xFF000000,
	0xFFAA0000,
	0xFF00AA00,
	0xFFFFFF00,
	0xFF0000AA,
	0xFFAA00AA,
	0xFF00AAAA,
	0xFFFFFFFF
};

uint32_t vconsole_ansi_color_pallet_bright[] = {
    0xFFAAAAAA,
    0xFFFF5555,
    0xFF55FF55,
    0xFFFFFF00,
    0xFF5555FF,
    0xFFFF55FF,
    0xFF00FFFF,
    0xFFFFFFFF
};

void vconsole_ansi_clear_from_cursor() {
    for (int i = vc_cursor_x; i < window->window_width; i++) {
        for (int j = vc_cursor_y; j < window->window_height; j++) {
        	set_pixel_window(i, j, vc_bgcolor);
    	}
	}
}

void vconsole_ansi_clear_from_cursor_eol() {
    for (int i = vc_cursor_x; i < window->window_width; i++) {
        for (int j = vc_cursor_y; j < vc_cursor_y + 16; j++) {
        	set_pixel_window(i, j, vc_bgcolor);
    	}
	}
}


#warning TODO: complete
void vconsole_ansi_run(char f, int* argv, int argc) {
    static bool bright = false;
	switch (f) {
		case 'm':
			{
				if (argc == 0) {
                    vc_bgcolor = 0;
                    vc_color = 0xffffffff;
                    bright = false;
				} else if (argc == 5 && argv[0] == 38 && argv[1] == 2) {
					vc_color = __builtin_bswap32(255 | (argv[2] << 8) | (argv[3] << 16) | (argv[4] << 24));
				} else {
					for (int i = 0; i < argc; i++) {
						switch (argv[i]) {
							case 0:
								{
                                    vc_bgcolor = 0;
                                    vc_color = 0xffffffff;
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
                                    vc_color = (bright ? vconsole_ansi_color_pallet_bright : vconsole_ansi_color_pallet)[argv[i] - 30];
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
                                    vc_bgcolor = (bright ? vconsole_ansi_color_pallet_bright : vconsole_ansi_color_pallet)[argv[i] - 40];
								}
                                break;
                            default:
                                vconsole_ansi_debug(f, argv, argc);
						}
					}
				}
			}
			break;
		case 'H':
			if (argc == 0) {
                vc_cursor_x = 0;
                vc_cursor_y = 0;
			} else if (argc == 2) {
                vc_cursor_x = (argv[1] - 1) * 8;
                vc_cursor_y = (argv[0] - 1) * 16;
			} else {
				vconsole_ansi_debug(f, argv, argc);
			}
			break;
		case 'J':
			if (argc == 0) {
				vconsole_ansi_clear_from_cursor();
			} else {
				vconsole_ansi_debug(f, argv, argc);
			}
			break;
		case 'K':
			if (argc == 0) {
				vconsole_ansi_clear_from_cursor_eol();
			} else {
			    vconsole_ansi_debug(f, argv, argc);
			}
			break;
		default:
			vconsole_ansi_debug(f, argv, argc);
			break;
	}
}

void vconsole_ansi_process_cmd(char* ansi) {
	assert(ansi[0] == 27);
	assert(ansi[1] == '[');

    ansi += 2;

	int args[32] = { 0 };
	int argc = 0;

	while (true) {
		if (*ansi == ';') {
			ansi++;
		} else if (isalpha(*ansi)) {
			vconsole_ansi_run(*ansi, args, argc);
			return;
		} else if (*ansi == 0) {
			return;
		} else if (!isdigit(*ansi)) {
			// printf("Malformed ansi: %c\n", *ansi);
			return;
		}
		ansi = __libc_parse_number(ansi, &args[argc++]);
	}
}

bool vconsole_ansi_process(char c) {
	static bool esc = false;
	static char esc_buf[32] = { 0 };
	static int esc_buf_idx = 0;
	if (c == 27) {
		esc = true;
	}

	if (!esc) {
		return false;
	} else {
		esc_buf[esc_buf_idx++] = c;
		if (isalpha(c)) {
			esc_buf[esc_buf_idx] = 0;
			esc = false;
			esc_buf_idx = 0;
			vconsole_ansi_process_cmd(esc_buf);
		}
        return true;
	}
}