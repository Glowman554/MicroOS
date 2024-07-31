#include <renderer/text_console.h>
#include <renderer/text_mode_emulation.h>

#include <stdint.h>
#include <utils/io.h>
#include <stdio.h>
#include <string.h>
#include <config.h>

int text_console_x = 0;
int text_console_y = 0;

#ifdef TEXT_MODE_EMULATION
char text_console_video[SCREEN_WIDTH * SCREEN_HEIGHT * 2] = { 0 };
#else
char* text_console_video = (char*) VIDEO_MEM;
#endif

uint32_t text_console_color = BACKGROUND_BLACK | FOREGROUND_WHITE;

void text_console_setcursor(uint16_t pos) {
#ifndef TEXT_MODE_EMULATION
	outb(0x3d4, 14);
	outb(0x3d5, pos >> 8);
	outb(0x3d4, 15);
	outb(0x3d5, pos);
#endif
}

void text_console_putc(char c) {
	if ((c == '\n') || (text_console_x > SCREEN_WIDTH - 1)) {
		text_console_x = 0;
		text_console_y++;
	}
	
	//if(SERIAL_DEBUG) write_serial(c);

	// if (c == '\n') {
	//	if(SERIAL_DEBUG) write_serial('\r');
		// return;
	// }

	if (c == '\r') {
		text_console_x = 0;
		return;
	}

	if (c == '\t') {
		if (text_console_x % 4 == 0) {
			text_console_x += 4;
		} else {
			text_console_x += (4 - (text_console_x % 4));
		}
		return;
	}

	if (c == '\b') {
		if (text_console_x > 0) {
			text_console_x--;
		}

		text_console_video[2 * (text_console_y * SCREEN_WIDTH + text_console_x)] = ' ';
		text_console_video[2 * (text_console_y * SCREEN_WIDTH + text_console_x) + 1] = text_console_color;
		text_console_setcursor(text_console_y * SCREEN_WIDTH + text_console_x);
        EMU_UPDATE();
		return;
	}

	if (text_console_y > SCREEN_HEIGHT - 1) {
		int i;
		for (i = 0; i < 2 * (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i++) {
			text_console_video[i] = text_console_video[i + SCREEN_WIDTH * 2];
		}

		for (; i < 2 * SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
			text_console_video[i] = 0;
		}
		text_console_y--;
	}

	if (c == '\n') {
        EMU_UPDATE();
		return;
	}

	text_console_video[2 * (text_console_y * SCREEN_WIDTH + text_console_x)] = c;
	text_console_video[2 * (text_console_y * SCREEN_WIDTH + text_console_x) + 1] = text_console_color;

	text_console_x++;

	text_console_setcursor(text_console_y * SCREEN_WIDTH + text_console_x);
    EMU_UPDATE();
}

void text_console_puts(const char *s) {
	while(*s) {
		text_console_putc(*s++);
	}
}

void text_console_clrscr() {
	int i;
	for (i = 0; i < 25 * 80; i++) {
		text_console_video[2*i] = ' ';
		text_console_video[2*i+1] = text_console_color;
	}

	text_console_x = text_console_y = 0;
    EMU_UPDATE();
}

bool text_console_driver_is_present(driver_t* driver) {
	return true;
}

char* text_console_driver_get_device_name(driver_t* driver) {
	return "text_console";
}

void text_console_driver_init(driver_t* driver) {
	// text_console_clrscr();

	printf_driver = (char_output_driver_t*) driver;
	global_char_output_driver = (char_output_driver_t*) driver;
}

void text_console_driver_putc(char_output_driver_t* driver, char c) {
	text_console_putc(c);
}

int text_console_driver_vmode(char_output_driver_t* driver) {
	return TEXT_80x25;
}

void text_console_driver_vpoke(char_output_driver_t* driver, uint32_t offset, uint8_t* val, uint32_t range) {
    memcpy(&text_console_video[offset], val, range);
    EMU_UPDATE();
}

void text_console_driver_vpeek(char_output_driver_t* driver, uint32_t offset, uint8_t* val, uint32_t range) {
    memcpy(val, &text_console_video[offset], range);
}

void text_console_vcursor(char_output_driver_t* driver, int x, int y) {
	text_console_x = x;
	text_console_y = y;
	text_console_setcursor(text_console_y * SCREEN_WIDTH + text_console_x);
}

void text_console_vcursor_get(char_output_driver_t* driver, int* x, int* y) {
	*x = text_console_x;
	*y = text_console_y;
}

char* color_table[] = {
	"black",
	"blue",
	"green",
	"cyan",
	"red",
	"magenta",
	"brown",
	"light_grey",
	"grey",
	"light_blue",
	"light_green",
	"light_cyan",
	"light_red",
	"light_magenta",
	"yellow",
	"white"
};

void text_console_set_color(char_output_driver_t* driver, char* color, bool background) {
	int i;
	for (i = 0; i < sizeof(color_table) / sizeof(color_table[0]); i++) {
		if (strcmp(color_table[i], color) == 0) {
			break;
		}
	}

	if (background) {
		text_console_color = i << 4 | (text_console_color & 0xf);
	} else {
		text_console_color = i | (text_console_color & 0xf0);
	}
}

char_output_driver_t text_console_driver = {
	.driver = {
		.is_device_present = text_console_driver_is_present,
		.get_device_name = text_console_driver_get_device_name,
		.init = text_console_driver_init
	},
	.putc = text_console_driver_putc,
	.vmode = text_console_driver_vmode,
	.vpoke = text_console_driver_vpoke,
    .vpeek = text_console_driver_vpeek,
	.vcursor = text_console_vcursor,
	.vcursor_get = text_console_vcursor_get,
	.set_color = text_console_set_color
};