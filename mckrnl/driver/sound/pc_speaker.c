#include <driver/sound/pc_speaker.h>
#include <utils/io.h>
#include <memory/vmm.h>
#include <stdio.h>
#include <driver/timer_driver.h>

uint16_t pc_speaker_notes_to_freq_table[7][12] = {
	{ 36485, 34437, 32505, 30680, 28958, 27333, 25799, 24351, 22984, 21694, 20477, 19327 },
	{ 18243, 17219, 16252, 15340, 14479, 13666, 12899, 12175, 11492, 10847, 10238, 9664 },
	{ 9121, 8609, 8126, 7670, 7240, 6833, 6450, 6088, 5746, 5424, 5119, 4832 },
	{ 4561, 4305, 4063, 3835, 3620, 3417, 3225, 3044, 2873, 2712, 2560, 2416 },
	{ 2280, 2152, 2032, 1918, 1810, 1708, 1612, 1522, 1437, 1356, 1280, 1208 },
	{ 1140, 1076, 1016, 959, 905, 854, 806, 761, 718, 678, 640, 604},
	{ 570, 538, 508, 479, 452, 427, 403, 380, 359, 339, 320, 302 }
};

void pc_speaker_turn_off() {
	outb(0x61, (inb(0x61) & 0xFC));
}

void pc_speaker_turn_on() {
	outb(0x61, (inb(0x61) | 3));
}

void pc_speaker_set_frequency(uint16_t frequency) {
	outb(0x43, 0xB6);
	outb(0x42, (uint8_t) frequency);
	outb(0x42, (uint8_t) (frequency >> 8));
}

uint16_t pc_speaker_note_to_frequency(uint8_t note) {
	uint8_t octave = DECODE_NOTE_OCTAVE(note);
	uint8_t note_number = DECODE_NOTE_NOTE(note);
	return pc_speaker_notes_to_freq_table[octave][note_number];
}

bool pc_speaker_is_device_present(driver_t* driver) {
	return true;
}

char* pc_speaker_get_device_name(driver_t* driver) {
	return "pc_speaker";
}

void pc_speaker_init(driver_t* driver) {
	sound_register_if_better((sound_driver_t*) driver);
	// sound_context_t* c = vmm_calloc(5);

	// int idx = 0;
	// for (int i = 0; i < 7; i++) {
	// 	for (int j = 0; j < 12; j++) {
	// 		c->notes[idx++] = (queued_note_t) {
	// 			.note = ENCODE_NOTE(i, j),
	// 			.duration_ms = 100
	// 		};
	// 	}
	// }
	// c->num_notes = idx;

	// coro_t coro = { 0 };
	// while (true) {
	// 	((sound_driver_t*) driver)->run((sound_driver_t*) driver, &coro, c);
	// }
}

void pc_speaker_run(sound_driver_t* driver, coro_t* coro, sound_context_t* context) {
	coro_begin(coro);

	if ((context->current_note % context->num_notes) == 0) {
		context->current_note = 0;
	}

	queued_note_t note = context->notes[context->current_note];
	debugf("Playing note %d for %d ms", note.note, note.duration_ms);
	if (note.note == 0) {
		pc_speaker_turn_off();
		pc_speaker_set_frequency(0);
	} else {
		pc_speaker_turn_on();
		pc_speaker_set_frequency(pc_speaker_note_to_frequency(note.note));
	}

	context->start_ms = global_timer_driver->time_ms(global_timer_driver);
	coro_cond_yield(coro, context->start_ms + context->notes[context->current_note].duration_ms < global_timer_driver->time_ms(global_timer_driver));

	context->current_note++;

	coro_reset(coro);
}

int pc_speaker_get_channels(sound_driver_t* driver) {
	return 1;
}

sound_driver_t pc_speaker_driver = {
	.driver = {
		.get_device_name = pc_speaker_get_device_name,
		.is_device_present = pc_speaker_is_device_present,
		.init = pc_speaker_init
	},
	.run = pc_speaker_run,
	.get_channels = pc_speaker_get_channels
};