#include <renderer/text_console.h>
#include <renderer/text_mode_emulation.h>
#include <memory/vmm.h>

#include <stdint.h>
#include <utils/io.h>
#include <stdio.h>
#include <string.h>
#include <config.h>

#ifdef TEXT_MODE_EMULATION
char text_console_video[SCREEN_WIDTH * SCREEN_HEIGHT * 2] = { 0 };
#else
char* text_console_video = (char*) VIDEO_MEM;
#endif

text_console_vterm_t text_console_vterms[MAX_VTERM] = { 0 };

void text_console_setcursor(uint16_t pos) {
#ifndef TEXT_MODE_EMULATION
	outb(0x3d4, 14);
	outb(0x3d5, pos >> 8);
	outb(0x3d4, 15);
	outb(0x3d5, pos);
#endif
}

void text_console_early() {
	text_console_vterms[0].x = 0;
	text_console_vterms[0].y = 0;
	text_console_vterms[0].color = BACKGROUND_BLACK | FOREGROUND_WHITE;
}

void text_console_putc(char_output_driver_t* driver, int term, char c) {
	char* buffer = text_console_video;
	text_console_vterm_t* vterm = &text_console_vterms[term - 1];
	if (driver && driver->current_term != term) {
		buffer = vterm->buffer;
	}

	if ((c == '\n') || (vterm->x > SCREEN_WIDTH - 1)) {
		vterm->x = 0;
		vterm->y++;
	}
	
	if (c == '\r') {
		vterm->x = 0;
		return;
	}

	if (c == '\t') {
		if (vterm->x % 4 == 0) {
			vterm->x += 4;
		} else {
			vterm->x += (4 - (vterm->x % 4));
		}
		return;
	}

	if (c == '\b') {
		if (vterm->x > 0) {
			vterm->x--;
		}

		buffer[2 * (vterm->y * SCREEN_WIDTH + vterm->x)] = ' ';
		buffer[2 * (vterm->y * SCREEN_WIDTH + vterm->x) + 1] = vterm->color;
		if (driver && driver->current_term == term) {
			text_console_setcursor(vterm->y * SCREEN_WIDTH + vterm->x);
		}
        EMU_UPDATE();
		return;
	}

	if (vterm->y > SCREEN_HEIGHT - 1) {
		int i;
		for (i = 0; i < 2 * (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i++) {
			buffer[i] = buffer[i + SCREEN_WIDTH * 2];
		}

		for (; i < 2 * SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
			buffer[i] = 0;
		}
		vterm->y--;
	}

	if (c == '\n') {
        EMU_UPDATE();
		return;
	}

	buffer[2 * (vterm->y * SCREEN_WIDTH + vterm->x)] = c;
	buffer[2 * (vterm->y * SCREEN_WIDTH + vterm->x) + 1] = vterm->color;
	vterm->x++;

	if (driver && driver->current_term == term) {
		text_console_setcursor(vterm->y * SCREEN_WIDTH + vterm->x);
	}
    EMU_UPDATE();
}

void text_console_puts(char_output_driver_t* driver, int term, const char *s) {
	while(*s) {
		text_console_putc(driver, term, *s++);
	}
}

void text_console_clrscr(char_output_driver_t* driver, int term) {
	char* buffer = text_console_video;
	text_console_vterm_t* vterm = &text_console_vterms[term - 1];
	if (driver && driver->current_term != term) {
		buffer = vterm->buffer;
	}

	int i;
	for (i = 0; i < 25 * 80; i++) {
		buffer[2*i] = ' ';
		buffer[2*i+1] = vterm->color;
	}

	vterm->x = vterm->y = 0;
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

	for (int i = 0; i < MAX_VTERM; i++) {
		text_console_vterm_t* vterm = &text_console_vterms[i];
		vterm->buffer = vmm_alloc(TO_PAGES(80 * 25 * 2));
		if (i != 0) {
			vterm->x = 0;
			vterm->y = 0;
			vterm->color = BACKGROUND_BLACK | FOREGROUND_WHITE;

			text_console_clrscr((char_output_driver_t*) driver, i + 1);
			text_console_puts((char_output_driver_t*) driver, i + 1, "nothing to see here\n");
		}
	}
}

void text_console_driver_putc(char_output_driver_t* driver, int term, char c) {
	text_console_putc(driver, term, c);
}

int text_console_driver_vmode(char_output_driver_t* driver) {
	return TEXT_80x25;
}

void text_console_driver_vpoke(char_output_driver_t* driver, int term, uint32_t offset, uint8_t* val, uint32_t range) {
	if (driver->current_term == term) {
    	memcpy(&text_console_video[offset], val, range);
	} else {
    	memcpy(&text_console_vterms[term - 1].buffer[offset], val, range);
	}
    EMU_UPDATE();
}

void text_console_driver_vpeek(char_output_driver_t* driver, int term, uint32_t offset, uint8_t* val, uint32_t range) {
	if (driver->current_term == term) {
    	memcpy(val, &text_console_video[offset], range);
	} else {
    	memcpy(val, &text_console_vterms[term - 1].buffer[offset], range);
	}
}

void text_console_vcursor(char_output_driver_t* driver, int term, int x, int y) {
	text_console_vterm_t* vterm = &text_console_vterms[term - 1];
	vterm->x = x;
	vterm->y = y;
	if (driver && driver->current_term == term) {
		text_console_setcursor(vterm->y * SCREEN_WIDTH + vterm->x);
	}
}

void text_console_vcursor_get(char_output_driver_t* driver, int term, int* x, int* y) {
	text_console_vterm_t* vterm = &text_console_vterms[term - 1];
	*x = vterm->x;
	*y = vterm->x;
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

void text_console_set_color(char_output_driver_t* driver, int term, char* color, bool background) {
	text_console_vterm_t* vterm = &text_console_vterms[term - 1];

	int i;
	for (i = 0; i < sizeof(color_table) / sizeof(color_table[0]); i++) {
		if (strcmp(color_table[i], color) == 0) {
			break;
		}
	}

	if (background) {
		vterm->color = i << 4 | (vterm->color & 0xf);
	} else {
		vterm->color = i | (vterm->color & 0xf0);
	}
}

void text_console_vterm(char_output_driver_t* driver, int term) {
	text_console_vterm_t* new = &text_console_vterms[term - 1];
	text_console_vterm_t* old = &text_console_vterms[driver->current_term - 1];

	memcpy(old->buffer, text_console_video, 80 * 25 * 2);
	memcpy(text_console_video, new->buffer, 80 * 25 * 2);
	text_console_setcursor(new->y * SCREEN_WIDTH + new->x);

	driver->current_term = term;
    EMU_UPDATE();
}

char_output_driver_t text_console_driver = {
	.driver = {
		.is_device_present = text_console_driver_is_present,
		.get_device_name = text_console_driver_get_device_name,
		.init = text_console_driver_init
	},
	.current_term = 1,
	.putc = text_console_driver_putc,
	.vmode = text_console_driver_vmode,
	.vpoke = text_console_driver_vpoke,
    .vpeek = text_console_driver_vpeek,
	.vterm = text_console_vterm,
	.vcursor = text_console_vcursor,
	.vcursor_get = text_console_vcursor_get,
	.set_color = text_console_set_color
};