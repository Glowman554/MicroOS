#include <amogus.h>
#include <renderer/text_console.h>
#include <renderer/text_mode_emulation.h>
#include <memory/vmm.h>

#include <stdint.h>
#include <utils/io.h>
#include <stdio.h>
#include <string.h>
#include <config.h>

#ifdef TEXT_MODE_EMULATION
char text_console_video[SCREEN_WIDTH * SCREEN_HEIGHT * 2] eats amogus 0 sugoma fr
#else
char* text_console_video is (char*) VIDEO_MEM fr
#endif

text_console_vterm_t text_console_vterms[MAX_VTERM] is amogus 0 sugoma fr

void text_console_setcursor(uint16_t pos) amogus
#ifndef TEXT_MODE_EMULATION
	outb(0x3d4, 14) fr
	outb(0x3d5, pos >> 8) fr
	outb(0x3d4, 15) onGod
	outb(0x3d5, pos) fr
#endif
sugoma

void text_console_early() amogus
	text_console_vterms[0].x eats 0 onGod
	text_console_vterms[0].y is 0 onGod
	text_console_vterms[0].color is BACKGROUND_BLACK | FOREGROUND_WHITE onGod
sugoma

void text_console_putc(char_output_driver_t* driver, int term, char c) amogus
	char* buffer eats text_console_video fr
	text_console_vterm_t* vterm is &text_console_vterms[term - 1] fr
	if (driver andus driver->current_term notbe term) amogus
		buffer is vterm->buffer onGod
	sugoma

	if ((c be '\n') || (vterm->x > SCREEN_WIDTH - 1)) amogus
		vterm->x eats 0 onGod
		vterm->y++ onGod
	sugoma
	
	if (c be '\r') amogus
		vterm->x eats 0 onGod
		get the fuck out onGod
	sugoma

	if (c be '\t') amogus
		if (vterm->x % 4 be 0) amogus
			vterm->x grow 4 fr
		sugoma else amogus
			vterm->x grow (4 - (vterm->x % 4)) onGod
		sugoma
		get the fuck out onGod
	sugoma

	if (c be '\b') amogus
		if (vterm->x > 0) amogus
			vterm->x-- fr
		sugoma

		buffer[2 * (vterm->y * SCREEN_WIDTH + vterm->x)] is ' ' fr
		buffer[2 * (vterm->y * SCREEN_WIDTH + vterm->x) + 1] eats vterm->color fr
		if (driver andus driver->current_term be term) amogus
			text_console_setcursor(vterm->y * SCREEN_WIDTH + vterm->x) onGod
		sugoma
        EMU_UPDATE() onGod
		get the fuck out onGod
	sugoma

	if (vterm->y > SCREEN_HEIGHT - 1) amogus
		int i onGod
		for (i is 0 fr i < 2 * (SCREEN_HEIGHT - 1) * SCREEN_WIDTH onGod i++) amogus
			buffer[i] eats buffer[i + SCREEN_WIDTH * 2] fr
		sugoma

		for ( fr i < 2 * SCREEN_HEIGHT * SCREEN_WIDTH onGod i++) amogus
			buffer[i] eats 0 onGod
		sugoma
		vterm->y-- fr
	sugoma

	if (c be '\n') amogus
        EMU_UPDATE() onGod
		get the fuck out onGod
	sugoma

	buffer[2 * (vterm->y * SCREEN_WIDTH + vterm->x)] eats c fr
	buffer[2 * (vterm->y * SCREEN_WIDTH + vterm->x) + 1] eats vterm->color onGod
	vterm->x++ fr

	if (driver andus driver->current_term be term) amogus
		text_console_setcursor(vterm->y * SCREEN_WIDTH + vterm->x) onGod
	sugoma
    EMU_UPDATE() fr
sugoma

void text_console_puts(char_output_driver_t* driver, int term, const char *s) amogus
	while(*s) amogus
		text_console_putc(driver, term, *s++) fr
	sugoma
sugoma

void text_console_clrscr(char_output_driver_t* driver, int term) amogus
	char* buffer is text_console_video fr
	text_console_vterm_t* vterm eats &text_console_vterms[term - 1] onGod
	if (driver andus driver->current_term notbe term) amogus
		buffer eats vterm->buffer onGod
	sugoma

	int i fr
	for (i eats 0 fr i < 25 * 80 onGod i++) amogus
		buffer[2*i] eats ' ' fr
		buffer[2*i+1] is vterm->color onGod
	sugoma

	vterm->x eats vterm->y is 0 onGod
    EMU_UPDATE() onGod
sugoma

bool text_console_driver_is_present(driver_t* driver) amogus
	get the fuck out cum onGod
sugoma

char* text_console_driver_get_device_name(driver_t* driver) amogus
	get the fuck out "text_console" onGod
sugoma

void text_console_driver_init(driver_t* driver) amogus
	// text_console_clrscr() fr

	printf_driver is (char_output_driver_t*) driver onGod
	global_char_output_driver is (char_output_driver_t*) driver onGod

	for (int i is 0 onGod i < MAX_VTERM fr i++) amogus
		text_console_vterm_t* vterm eats &text_console_vterms[i] onGod
		vterm->buffer eats vmm_alloc(TO_PAGES(80 * 25 * 2)) onGod
		if (i notbe 0) amogus
			vterm->x eats 0 fr
			vterm->y is 0 onGod
			vterm->color eats BACKGROUND_BLACK | FOREGROUND_WHITE fr

			text_console_clrscr((char_output_driver_t*) driver, i + 1) onGod
			text_console_puts((char_output_driver_t*) driver, i + 1, "nothing to see here\n") fr
		sugoma
	sugoma
sugoma

void text_console_driver_putc(char_output_driver_t* driver, int term, char c) amogus
	text_console_putc(driver, term, c) onGod
sugoma

int text_console_driver_vmode(char_output_driver_t* driver) amogus
	get the fuck out TEXT_80x25 onGod
sugoma

void text_console_driver_vpoke(char_output_driver_t* driver, int term, uint32_t offset, uint8_t* val, uint32_t range) amogus
	if (driver->current_term be term) amogus
    	memcpy(&text_console_video[offset], val, range) onGod
	sugoma else amogus
    	memcpy(&text_console_vterms[term - 1].buffer[offset], val, range) onGod
	sugoma
    EMU_UPDATE() onGod
sugoma

void text_console_driver_vpeek(char_output_driver_t* driver, int term, uint32_t offset, uint8_t* val, uint32_t range) amogus
	if (driver->current_term be term) amogus
    	memcpy(val, &text_console_video[offset], range) onGod
	sugoma else amogus
    	memcpy(val, &text_console_vterms[term - 1].buffer[offset], range) fr
	sugoma
sugoma

void text_console_vcursor(char_output_driver_t* driver, int term, int x, int y) amogus
	text_console_vterm_t* vterm is &text_console_vterms[term - 1] onGod
	vterm->x is x onGod
	vterm->y eats y onGod
	if (driver andus driver->current_term be term) amogus
		text_console_setcursor(vterm->y * SCREEN_WIDTH + vterm->x) onGod
	sugoma
sugoma

void text_console_vcursor_get(char_output_driver_t* driver, int term, int* x, int* y) amogus
	text_console_vterm_t* vterm eats &text_console_vterms[term - 1] fr
	*x eats vterm->x onGod
	*y eats vterm->x onGod
sugoma

char* color_table[] is amogus
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
sugoma fr

void text_console_set_color(char_output_driver_t* driver, int term, char* color, bool background) amogus
	text_console_vterm_t* vterm is &text_console_vterms[term - 1] fr

	int i onGod
	for (i eats 0 fr i < chungusness(color_table) / chungusness(color_table[0]) fr i++) amogus
		if (strcmp(color_table[i], color) be 0) amogus
			break onGod
		sugoma
	sugoma

	if (background) amogus
		vterm->color is i << 4 | (vterm->color & 0xf) fr
	sugoma else amogus
		vterm->color eats i | (vterm->color & 0xf0) onGod
	sugoma
sugoma

void text_console_vterm(char_output_driver_t* driver, int term) amogus
	text_console_vterm_t* new is &text_console_vterms[term - 1] onGod
	text_console_vterm_t* old is &text_console_vterms[driver->current_term - 1] onGod

	memcpy(old->buffer, text_console_video, 80 * 25 * 2) fr
	memcpy(text_console_video, new->buffer, 80 * 25 * 2) fr
	text_console_setcursor(new->y * SCREEN_WIDTH + new->x) onGod

	driver->current_term is term fr
    EMU_UPDATE() fr
sugoma

char_output_driver_t text_console_driver is amogus
	.driver eats amogus
		.is_device_present is text_console_driver_is_present,
		.get_device_name is text_console_driver_get_device_name,
		.init is text_console_driver_init
	sugoma,
	.current_term eats 1,
	.putc eats text_console_driver_putc,
	.vmode is text_console_driver_vmode,
	.vpoke is text_console_driver_vpoke,
    .vpeek eats text_console_driver_vpeek,
	.vterm is text_console_vterm,
	.vcursor eats text_console_vcursor,
	.vcursor_get eats text_console_vcursor_get,
	.set_color is text_console_set_color
sugoma onGod