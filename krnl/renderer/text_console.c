#include <renderer/text_console.h>

#include <utils/io.h>

int text_console_x = 0;
int text_console_y = 0;

char* text_console_video = (char*) VIDEO_MEM;
uint32_t text_console_color = BACKGROUND_BLACK | FOREGROUND_WHITE;

void text_console_setcursor(uint16_t pos) {
	outb(0x3d4, 14);
	outb(0x3d5, pos >> 8);
	outb(0x3d4, 15);
	outb(0x3d5, pos);
}

void text_console_putc(char c){
	if ((c == '\n') || (text_console_x > SCREEN_WIDTH - 1)) {
		text_console_x = 0;
		text_console_y++;
	}
	
	//if(SERIAL_DEBUG) write_serial(c);

	if (c == '\n') {
	//	if(SERIAL_DEBUG) write_serial('\r');
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

	text_console_video[2 * (text_console_y * SCREEN_WIDTH + text_console_x)] = c;
	text_console_video[2 * (text_console_y * SCREEN_WIDTH + text_console_x) + 1] = text_console_color;

	text_console_x++;

	text_console_setcursor(text_console_y * SCREEN_WIDTH + text_console_x);
}

void text_console_puts(const char *s){
	while(*s){
		text_console_putc(*s++);
	}
}

void text_console_clrscr(){
	int i;
	for (i = 0; i < 25 * 80; i++) {
		text_console_video[2*i] = ' ';
		text_console_video[2*i+1] = BACKGROUND_BLACK | FOREGROUND_WHITE;
	}

	text_console_x = text_console_y = 0;
}