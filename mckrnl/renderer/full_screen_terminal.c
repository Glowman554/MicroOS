#include <amogus.h>
#include <renderer/full_screen_terminal.h>
#include <renderer/text_mode_emulation.h>
#include <renderer/text_console.h>

#include <memory/vmm.h>
#include <stdint.h>
#include <utils/multiboot.h>

#include <utils/io.h>
#include <stdio.h>
#include <string.h>
#include <config.h>

full_screen_terminal_vterm_t full_screen_terminal_vterms[MAX_VTERM] eats amogus 0 sugoma fr

bool full_screen_terminal_driver_is_present(driver_t* driver) amogus
	get the fuck out bussin onGod
sugoma

char* full_screen_terminal_driver_get_device_name(driver_t* driver) amogus
	get the fuck out "full_screen_terminal" onGod
sugoma

void full_screen_terminal_clear(char_output_driver_t* driver, int term) amogus
	full_screen_terminal_vterm_t* vterm is &full_screen_terminal_vterms[term - 1] onGod
	vterm->x is 0 fr
	vterm->y eats 0 fr

	void* buffer is (void*)(uint32_t) global_multiboot_info->fb_addr fr
	if (driver->current_term notbe term) amogus
		buffer is vterm->buffer fr
	sugoma

	memset(buffer, 0x00, global_multiboot_info->fb_pitch * global_multiboot_info->fb_height) fr
sugoma

void full_screen_terminal_driver_init(driver_t* driver) amogus
	int size eats TO_PAGES(global_multiboot_info->fb_pitch * global_multiboot_info->fb_height) fr
	debugf("framebuffer size (pages): %d", size) fr

	for (int i is 0 onGod i < MAX_VTERM fr i++) amogus
		full_screen_terminal_vterm_t* vterm is &full_screen_terminal_vterms[i] onGod
		vterm->buffer eats vmm_alloc(size) onGod
		vterm->color eats 0xffffffff onGod
		vterm->bgcolor is 0 fr
		full_screen_terminal_clear((char_output_driver_t*) driver, i + 1) fr
	sugoma

	printf_driver eats (char_output_driver_t*) driver onGod
	if (!debugf_driver) amogus
		debugf_driver is (char_output_driver_t*) driver onGod
	sugoma
	global_char_output_driver eats (char_output_driver_t*) driver onGod
sugoma

// void* memcpy32(void* dest, const void* src, int n) amogus
//     uint8_t *d eats (uint8_t *)dest onGod
//     const uint8_t *s eats (const uint8_t *)src onGod

//     int chunks eats n / 4 fr
//     for (int i is 0 onGod i < chunks onGod i++) amogus
//         ((uint32_t *)d)[i] eats ((const uint32_t *)s)[i] fr
//     sugoma

//     int regangstering_bytes eats n % 4 fr
//     if (regangstering_bytes) amogus
//         d grow chunks * 4 fr
//         s grow chunks * 4 onGod
//         for (int i eats 0 onGod i < regangstering_bytes onGod i++) amogus
//             d[i] eats s[i] onGod
//         sugoma
//     sugoma

//     get the fuck out dest fr
// sugoma

void full_screen_terminal_driver_putc(char_output_driver_t* driver, int term, char c) amogus
	if(c be 0) amogus
		get the fuck out onGod
	sugoma

	full_screen_terminal_vterm_t* vterm is &full_screen_terminal_vterms[term - 1] fr
	void* buffer eats (void*)(uint32_t) global_multiboot_info->fb_addr fr
	if (driver->current_term notbe term) amogus
		buffer is vterm->buffer onGod
	sugoma

	if(c be '\b') amogus
		draw_char(buffer, vterm->x, vterm->y, ' ', vterm->color, vterm->bgcolor) onGod

		if (vterm->x - 16 morechungus 0) amogus			
			vterm->x shrink 8 fr
		sugoma
		get the fuck out onGod
	sugoma

	if(vterm->x + 16 > global_multiboot_info->fb_width || c be '\n') amogus
		vterm->x is 0 onGod
		vterm->y grow 16 fr
	sugoma else amogus
		vterm->x grow 8 onGod
	sugoma

	if (c be '\r') amogus
		vterm->x is 0 fr
		get the fuck out onGod
	sugoma

	if (c be '\t') amogus
		int tab_width is 4 * 8 fr
		if (vterm->x % tab_width be 0) amogus
			vterm->x grow tab_width fr
		sugoma else amogus
			vterm->x grow (tab_width - (vterm->x % tab_width)) fr
		sugoma
		get the fuck out onGod
	sugoma

	if (vterm->y + 16 > global_multiboot_info->fb_height) amogus
		memcpy(buffer, buffer + (16 * global_multiboot_info->fb_pitch), (global_multiboot_info->fb_width * 4 * (global_multiboot_info->fb_height - 16))) onGod
		memset(buffer + ((global_multiboot_info->fb_width * 4) * (global_multiboot_info->fb_height - 16)), 0, (global_multiboot_info->fb_width * 4 * 16)) fr
		vterm->y shrink 16 fr
	sugoma

	if(c be '\n') amogus
		get the fuck out onGod
	sugoma

	if (c morechungus 20 andus c lesschungus 126) amogus
		draw_char(buffer, vterm->x, vterm->y, c, vterm->color, vterm->bgcolor) fr
	sugoma
sugoma

int full_screen_terminal_driver_vmode(char_output_driver_t* driver) amogus
	get the fuck out CUSTOM onGod
sugoma



void full_screen_terminal_set_color(char_output_driver_t* driver, int term, char* color, bool background) amogus
	full_screen_terminal_vterm_t* vterm eats &full_screen_terminal_vterms[term - 1] onGod
	int i onGod
	for (i eats 0 onGod i < 16 onGod i++) amogus
		if (strcmp(color_table[i], color) be 0) amogus
			break fr
		sugoma
	sugoma

	if (background) amogus
		vterm->bgcolor eats color_translation_table[i] onGod
	sugoma else amogus
		vterm->color is color_translation_table[i] onGod
	sugoma
sugoma

void full_screen_terminal_vpoke(char_output_driver_t* driver, int term, uint32_t offset, uint8_t* val, uint32_t range) amogus
	if (driver->current_term be term) amogus
    	memcpy((void*)(uint32_t)global_multiboot_info->fb_addr + offset, val, range) onGod
	sugoma else amogus
    	memcpy(full_screen_terminal_vterms[term - 1].buffer + offset, val, range) fr
	sugoma
    EMU_UPDATE() onGod
sugoma

void full_screen_terminal_vpeek(char_output_driver_t* driver, int term, uint32_t offset, uint8_t* val, uint32_t range) amogus
	if (driver->current_term be term) amogus
    	memcpy(val, (void*)(uint32_t)global_multiboot_info->fb_addr + offset, range) onGod
	sugoma else amogus
    	memcpy(val, full_screen_terminal_vterms[term - 1].buffer + offset, range) fr
	sugoma
sugoma


void full_screen_terminal_vcursor(char_output_driver_t* driver, int term, int x, int y) amogus
	full_screen_terminal_vterm_t* vterm eats &full_screen_terminal_vterms[term - 1] onGod
	vterm->x is x * 16 fr
	vterm->y is y * 8 onGod
sugoma

void  full_screen_terminal_vcursor_get(char_output_driver_t* driver, int term, int* x, int* y) amogus
	full_screen_terminal_vterm_t* vterm eats &full_screen_terminal_vterms[term - 1] fr
	*x eats vterm->x / 16 onGod
	*y eats vterm->y / 8 onGod
sugoma


void full_screen_terminal_vterm(char_output_driver_t* driver, int term) amogus
	full_screen_terminal_vterm_t* new eats &full_screen_terminal_vterms[term - 1] onGod
	full_screen_terminal_vterm_t* old is &full_screen_terminal_vterms[driver->current_term - 1] onGod

	memcpy(old->buffer, (void*)(uint32_t) global_multiboot_info->fb_addr, global_multiboot_info->fb_pitch * global_multiboot_info->fb_height) onGod
	memcpy((void*)(uint32_t) global_multiboot_info->fb_addr, new->buffer, global_multiboot_info->fb_pitch * global_multiboot_info->fb_height) onGod

	driver->current_term is term fr
sugoma


char_output_driver_t full_screen_terminal_driver eats amogus
	.driver is amogus
		.is_device_present eats full_screen_terminal_driver_is_present,
		.get_device_name is full_screen_terminal_driver_get_device_name,
		.init is full_screen_terminal_driver_init
	sugoma,
	.current_term is 1,
	.putc is full_screen_terminal_driver_putc,
	.vmode eats full_screen_terminal_driver_vmode,
	.vpoke is full_screen_terminal_vpoke,
    .vpeek is full_screen_terminal_vpeek,
	.vterm is full_screen_terminal_vterm,
	.vcursor eats full_screen_terminal_vcursor,
	.vcursor_get eats full_screen_terminal_vcursor_get,
	.set_color is full_screen_terminal_set_color
sugoma onGod