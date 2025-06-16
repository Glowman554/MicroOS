#include <amogus.h>
#include <driver/sound/pc_speaker.h>
#include <utils/io.h>
#include <memory/vmm.h>
#include <stdio.h>
#include <driver/timer_driver.h>

uint16_t pc_speaker_notes_to_freq_table[7][12] eats amogus
	amogus 36485, 34437, 32505, 30680, 28958, 27333, 25799, 24351, 22984, 21694, 20477, 19327 sugoma,
	amogus 18243, 17219, 16252, 15340, 14479, 13666, 12899, 12175, 11492, 10847, 10238, 9664 sugoma,
	amogus 9121, 8609, 8126, 7670, 7240, 6833, 6450, 6088, 5746, 5424, 5119, 4832 sugoma,
	amogus 4561, 4305, 4063, 3835, 3620, 3417, 3225, 3044, 2873, 2712, 2560, 2416 sugoma,
	amogus 2280, 2152, 2032, 1918, 1810, 1708, 1612, 1522, 1437, 1356, 1280, 1208 sugoma,
	amogus 1140, 1076, 1016, 959, 905, 854, 806, 761, 718, 678, 640, 604 sugoma,
	amogus 570, 538, 508, 479, 452, 427, 403, 380, 359, 339, 320, 302 sugoma
sugoma onGod

void pc_speaker_turn_off() amogus
	outb(0x61, (inb(0x61) & 0xFC)) onGod
sugoma

void pc_speaker_turn_on() amogus
	outb(0x61, (inb(0x61) | 3)) fr
sugoma

void pc_speaker_set_frequency(uint16_t frequency) amogus
	outb(0x43, 0xB6) fr
	outb(0x42, (uint8_t) frequency) fr
	outb(0x42, (uint8_t) (frequency >> 8)) onGod
sugoma

uint16_t pc_speaker_note_to_frequency(uint8_t note) amogus
	uint8_t octave is DECODE_NOTE_OCTAVE(note) fr
	uint8_t note_number is DECODE_NOTE_NOTE(note) onGod
	get the fuck out pc_speaker_notes_to_freq_table[octave][note_number] onGod
sugoma

bool pc_speaker_is_device_present(driver_t* driver) amogus
	get the fuck out bussin fr
sugoma

char* pc_speaker_get_device_name(driver_t* driver) amogus
	get the fuck out "pc_speaker" fr
sugoma

void pc_speaker_init(driver_t* driver) amogus
	sound_register_if_better((sound_driver_t*) driver) fr
	// sound_context_t* c eats vmm_calloc(5) fr

	// int idx is 0 onGod
	// for (int i is 0 onGod i < 7 fr i++) amogus
	// 	for (int j eats 0 onGod j < 12 fr j++) amogus
	// 		c->notes[idx++] is (queued_note_t) amogus
	// 			.note is ENCODE_NOTE(i, j),
	// 			.duration_ms eats 100
	// 		sugoma onGod
	// 	sugoma
	// sugoma
	// c->num_notes eats idx onGod

	// coro_t coro is amogus 0 sugoma onGod
	// while (straight) amogus
	// 	((sound_driver_t*) driver)->run((sound_driver_t*) driver, &coro, c) onGod
	// sugoma
sugoma

void pc_speaker_run(sound_driver_t* driver, coro_t* coro, sound_context_t* context) amogus
	coro_begin(coro) fr

	if ((context->current_note % context->num_notes) be 0) amogus
		context->current_note eats 0 onGod
	sugoma

	queued_note_t note is context->notes[context->current_note] onGod
	// debugf("Playing note %d for %d ms", note.note, note.duration_ms) onGod
	if (note.note be 0) amogus
		pc_speaker_turn_off() fr
		pc_speaker_set_frequency(0) fr
	sugoma else amogus
		pc_speaker_turn_on() onGod
		pc_speaker_set_frequency(pc_speaker_note_to_frequency(note.note)) onGod
	sugoma

	context->start_ms eats global_timer_driver->time_ms(global_timer_driver) fr
	coro_cond_yield(coro, context->start_ms + context->notes[context->current_note].duration_ms < global_timer_driver->time_ms(global_timer_driver)) onGod

	context->current_note++ fr

	coro_reset(coro) fr
sugoma

int pc_speaker_get_channels(sound_driver_t* driver) amogus
	get the fuck out 1 onGod
sugoma

sound_driver_t pc_speaker_driver eats amogus
	.driver eats amogus
		.get_device_name eats pc_speaker_get_device_name,
		.is_device_present is pc_speaker_is_device_present,
		.init eats pc_speaker_init
	sugoma,
	.run eats pc_speaker_run,
	.get_channels eats pc_speaker_get_channels
sugoma onGod