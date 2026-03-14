#include "driver/char_output_driver.h"
#include <renderer/ansi.h>
#include <stdio.h>

#define isupper(c) (c >= 'A' && c <= 'Z')
#define islower(c) (c >= 'a' && c <= 'z')
#define isalpha(c) (islower(c) || isupper(c))
#define isdigit(c) (c >= '0' && c <= '9')
#define MIN(a, b) ((a) < (b) ? (a) : (b))


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

char* parse_number(char* input, int* output) {
	int idx = 0;
	int number_system_base = 10;

	if (input[0] == '0') {
		if (input[1] == 'x') {
			number_system_base = 16;
			idx = 2;
		} else if (input[1] == 'b') {
			number_system_base = 2;
			idx = 2;
		}
	}

	int _number = 0;

	while (input[idx] != '\0') {
		if (input[idx] >= '0' && input[idx] <= '9') {
			_number = _number * number_system_base + (input[idx] - '0');
		} else if (input[idx] >= 'a' && input[idx] <= 'f') {
			_number = _number * number_system_base + (input[idx] - 'a' + 10);
		} else if (input[idx] >= 'A' && input[idx] <= 'F') {
			_number = _number * number_system_base + (input[idx] - 'A' + 10);
		} else {
			break;
		}

		idx++;
	}

	*output = _number;

	return &input[idx];
}

void ansi_debug(char f, int* argv, int argc) {
	char buffer[32] = { 0 };
	char* c = buffer;

	c += sprintf(c, "%c(", f);
	
	for (int i = 0; i < argc; i++) {
		if (i + 1 < argc) {
			c += sprintf(c, "%d, ", argv[i]);
		} else {
			c += sprintf(c, "%d)", argv[i]);
		}
	}

	if (argc == 0) {
		c += sprintf(c, ")");
	}

	debugf(INFO, "ansi: %s", buffer);
}

void ansi_run(char_output_driver_t* driver, int term, char f, int* argv, int argc) {
	switch (f) {
		case 'm':
			{
				if (argc == 0) {
					driver->set_color(driver, term, "black", true);
					driver->set_color(driver, term, "white", false);
                    driver->ansi_bright[term] = false;
				} else {
					for (int i = 0; i < argc; i++) {
						switch (argv[i]) {
							case 0:
								{
									driver->set_color(driver, term, "black", true);
									driver->set_color(driver, term, "white", false);
				                    driver->ansi_bright[term] = false;
								}
								break;
                            case 1: 
				                driver->ansi_bright[term] = true;
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
									driver->set_color(driver, term, (driver->ansi_bright[term] ? color_pallet_bright : color_pallet)[argv[i] - 30], false);
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
                                    driver->set_color(driver, term, color_pallet[argv[i] - 40], true);
									driver->set_color(driver, term, (driver->ansi_bright[term] ? color_pallet_bright : color_pallet)[argv[i] - 40], true);
								}
                                break;
                            // default:
                            //     ansi_debug(f, argv, argc);
						}
					}
				}
			}
			break;
		case 'H':
			{
				int x = 0, y = 0;
				if (argc == 2) {
					x = MIN(25, argv[1]) - 1;
					y = MIN(80, argv[0]) - 1;
				}

				driver->vcursor(driver, term, x, y);
			}
			break;
		case 'J':
			{
				enum ansi_erase_mode erase_mode = ERASE_CURSOR_TO_END; 
				if (argc >= 1) {
					erase_mode = (enum ansi_erase_mode) argv[0];
				}
				
				driver->erase_display(driver, term, erase_mode);
			}
			break;
		case 'K':
			{
				enum ansi_erase_mode erase_mode = ERASE_CURSOR_TO_END; 
				if (argc >= 1) {
					erase_mode = (enum ansi_erase_mode) argv[0];
				}
				
				driver->erase_line(driver, term, erase_mode);
			}
			break;
		// default:
		// 	ansi_debug(f, argv, argc);
		// 	break;
	}
}



void ansi_process_args(char_output_driver_t* driver, int term, char* ansi) {
	if (ansi[0] != 27 || ansi[1] != '[') {
		// debugf("Invalid ansi start");
		return;
	}

    ansi += 2;

	int args[32] = { 0 };
	int argc = 0;

	while (true) {
		if (*ansi == ';') {
			ansi++;
		} else if (isalpha(*ansi)) {
			ansi_run(driver, term, *ansi, args, argc);
			return;
		} else if (*ansi == 0) {
			return;
		} else if (!isdigit(*ansi)) {
			// debugf("Malformed ansi: %c", *ansi);
			return;
		}
		ansi = parse_number(ansi, &args[argc++]);
	}
}

char ansi_process(char_output_driver_t* driver, int term, char c) {
	if (!driver) {
		return c;
	}
	
    if (c == 27) {
		driver->ansi_esc[term] = true;
	}

	if (driver->ansi_esc[term]) {
		driver->ansi_esc_buffer[term][driver->ansi_esc_buffer_index[term]++] = c;
		if (isalpha(c)) {
			driver->ansi_esc_buffer[term][driver->ansi_esc_buffer_index[term]] = 0;
			driver->ansi_esc[term] = false;
			driver->ansi_esc_buffer_index[term] = 0;

			ansi_process_args(driver, term, driver->ansi_esc_buffer[term]);
		}
	} else {
        return c;
    }

    return 0;
}